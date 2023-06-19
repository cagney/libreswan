ipsec whack --impair revival --impair emitting

# bring up west and confirm
ipsec auto --up west
../../guestbin/ping-once.sh --up -I 192.0.1.254 192.0.2.254
ipsec whack --trafficstatus
ipsec whack --rekey-ipsec --name west
ipsec auto --down west

# bogus empty SPI
ipsec auto --up west
../../guestbin/ping-once.sh --up -I 192.0.1.254 192.0.2.254
ipsec whack --trafficstatus
ipsec whack --impair v2n_rekey_sa_spi_size:0  --impair v2n_rekey_sa_body:no
ipsec whack --rekey-ipsec --name west
ipsec whack --impair v2n_rekey_sa_spi_size:no --impair v2n_rekey_sa_body:no
ipsec auto --down west

# bogus empty SPI
ipsec auto --up west
../../guestbin/ping-once.sh --up -I 192.0.1.254 192.0.2.254
ipsec whack --trafficstatus
ipsec whack --impair v2n_rekey_sa_spi_size:0  --impair v2n_rekey_sa_body:0
ipsec whack --rekey-ipsec --name west
ipsec whack --impair v2n_rekey_sa_spi_size:no --impair v2n_rekey_sa_body:no
ipsec auto --down west

# bogus huge SPI
ipsec auto --up west
../../guestbin/ping-once.sh --up -I 192.0.1.254 192.0.2.254
ipsec whack --trafficstatus
ipsec whack --impair v2n_rekey_sa_spi_size:254 --impair v2n_rekey_sa_body:no
ipsec whack --rekey-ipsec --name west
ipsec whack --impair v2n_rekey_sa_spi_size:no  --impair v2n_rekey_sa_body:no
ipsec auto --down west

# bogus truely huge SPI
ipsec auto --up west
../../guestbin/ping-once.sh --up -I 192.0.1.254 192.0.2.254
ipsec whack --trafficstatus
ipsec whack --impair v2n_rekey_sa_spi_size:254 --impair v2n_rekey_sa_body:60000
ipsec whack --rekey-ipsec --name west
ipsec whack --impair v2n_rekey_sa_spi_size:no  --impair v2n_rekey_sa_body:no
ipsec auto --down west
