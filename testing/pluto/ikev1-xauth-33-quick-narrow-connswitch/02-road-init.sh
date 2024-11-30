../../guestbin/swan-prep --x509

ipsec start
../../guestbin/wait-until-pluto-started

ipsec whack --impair suppress_retransmits
ipsec whack --impair revival

ipsec add wide-road-to-east
ipsec add narrow-road-to-east
ipsec add dirt-road-to-east
