# The CREATE_CHILD_SA response needs to finish DH so sent to the
# crypto helper, and the IKE SA is suspended

../../guestbin/drip-inbound.sh 1 '#3: IMPAIR: job 4 .* helper is pausing for 5 seconds'

# The CREATE_CHILD_SA request also needs to compute DH so is also sent
# to the crypto helper, and again the IKE SA is suspended.

../../guestbin/drip-inbound.sh 2 '#4: responder established Child SA using #1'

ipsec whack --no-impair block_inbound
ipsec whack --no-impair helper_thread_delay
