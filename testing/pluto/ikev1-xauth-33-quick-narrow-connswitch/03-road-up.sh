ipsec whack --xauthname 'xroad' --xauthpass 'use1pass' --name wide-road-to-east --initiate
../../guestbin/ping-once.sh --up -I 192.0.2.101 192.0.2.254
ipsec whack --trafficstatus
ipsec down wide-road-to-east
ipsec delete wide-road-to-east

ipsec whack --xauthname 'xroad' --xauthpass 'use1pass' --name narrow-road-to-east --initiate
../../guestbin/ping-once.sh --up -I 192.0.2.101 192.0.2.254
ipsec whack --trafficstatus
ipsec down narrow-road-to-east
ipsec delete narrow-road-to-east

ipsec whack --xauthname 'xroad' --xauthpass 'use1pass' --name dirt-road-to-east --initiate
../../guestbin/ping-once.sh --up -I 192.0.2.101 192.0.2.254
ipsec whack --trafficstatus
ipsec down dirt-road-to-east
ipsec delete dirt-road-to-east
