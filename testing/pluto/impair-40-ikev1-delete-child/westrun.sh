ipsec auto --up west-east
# send to delete child SAs
ipsec whack --impair v1_isakmp_delete_payload:emit_duplicate
ipsec whack --impair v1_ipsec_delete_payload:emit_duplicate
ipsec auto --delete west-east
