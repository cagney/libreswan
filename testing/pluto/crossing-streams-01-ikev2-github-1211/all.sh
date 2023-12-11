#
# set up east
#

east# /testing/guestbin/swan-prep
east# ipsec start


east# ../../guestbin/wait-until-pluto-started
east# ipsec whack --impair revival
east# ipsec whack --impair suppress-retransmits
east# ipsec auto --add east-west


#
# set up west
#

west# /testing/guestbin/swan-prep
west# ipsec start


west# ../../guestbin/wait-until-pluto-started
east# ipsec whack --impair revival
west# ipsec whack --impair suppress-retransmits
west# ipsec whack --impair block-inbound:yes


west# ipsec auto --add east-west


#
# initiate east
#
# hold east's IKE_SA_INIT request as inbound message 1

east# ipsec up --asynchronous east-west


west# ../../guestbin/wait-for.sh --match 'IMPAIR: blocking inbound message 1' -- cat /tmp/pluto.log


#
# initiate west (create IKE #1)
#
# hold east's IKE_SA_INIT response as inbound message 2

west# ipsec up --asynchronous east-west


west# ../../guestbin/wait-for.sh --match '^".*#1: sent IKE_SA_INIT request' -- cat /tmp/pluto.log


west# ../../guestbin/wait-for.sh --match 'IMPAIR: blocking inbound message 2' -- cat /tmp/pluto.log


#
# on west, respond to east's IKE_SA_INIT request (message 1) (create IKE #2)
#
# hold east's IKE_AUTH request as inbound message 3

west# ipsec whack --impair drip-inbound:1


west# ../../guestbin/wait-for.sh --match '^".*#2: sent IKE_SA_INIT' -- cat /tmp/pluto.log


west# ../../guestbin/wait-for.sh --match 'IMPAIR: blocking inbound message 3' -- cat /tmp/pluto.log


#
# on west, process east's IKE_SA_INIT response (message 2) (create Child #3)
#
# hold east's IKE_AUTH response as inbound message 4

west# ipsec whack --impair drip-inbound:2


west# ../../guestbin/wait-for.sh --match '^".*#1: sent IKE_AUTH request'  -- cat /tmp/pluto.log


west# ../../guestbin/wait-for.sh --match 'IMPAIR: blocking inbound message 4'  -- cat /tmp/pluto.log


#
# on west, process east's IKE_SA_AUTH response (message 4)
#
# it should establish

west# ipsec whack --impair drip-inbound:4


west# ../../guestbin/wait-for.sh --match 'established Child SA using #1' -- cat /tmp/pluto.log


#
# on west, process east's IKE_SA_AUTH request (message 3) (create Child #4)
#
# it should establish

west# ipsec whack --impair drip-inbound:3


west# ../../guestbin/wait-for.sh --match 'established Child SA using #2'  -- cat /tmp/pluto.log


#
# On east, delete the peer's IKE SA
#

west# ipsec whack --impair block-inbound:no


east# ipsec showstates
east# ipsec down '#1'
