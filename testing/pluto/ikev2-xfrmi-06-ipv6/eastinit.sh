/testing/guestbin/swan-prep --46
# confirm that the network is alive
ipsec start
../../guestbin/wait-until-pluto-started
ipsec auto --add eastnet-road
echo "initdone"
