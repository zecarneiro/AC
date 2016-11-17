	# memory mapped keyboard and display example
	# PM
	.data
ALL_INT_MASK: .word 0x0000ff00
KBD_INT_MASK: .word 0x00000100
RCR:		.word 0xffff0000
msg: .asciiz "Memory mapped keyboard and display example by\nPM\n
	"
	.text	
main:
	jal int_enable
	
	la   $a0, msg  # address of string to print
  	li   $v0, 4    # Print String service
  	syscall
forever:
	j forever
		
int_enable:
	mfc0 $t0, $12
	lw $t1, ALL_INT_MASK
	not $t1,$t1
	and $t0,$t0,$t1 # disable all int 
	lw $t1, KBD_INT_MASK	
	or $t0,$t0,$t1
	mtc0 $t0,$12
	
	# now enable interrupts on the KBD
	lw $t0,RCR
	li $t1, 0x00000002
	sw $t1 0($t0)
	jr $ra

	.kdata 
save_t1: .word
save_t2: .word
save_s1: .word
save_s2: .word
save_s3: .word
save_s4: .word
save_a0: .word
save_v0: .word
save_at: .word

RXD:	.word 0xffff0004
TXD:	.word 0xffff000c

	.ktext 0x80000180
	
	#save every used register as needed
	move $k0,$at
	sw $k0, save_at
	
	sw $t1, save_t1
	sw $t2, save_t2
	sw $s1, save_s1
	sw $s2, save_s2
	sw $s3, save_s3
	sw $s4, save_s4
	sw $a0, save_a0
	sw $v0, save_v0
	
	mfc0 $k0,$13 # get cause register 
	srl $t1,$k0,2
	andi $t1,$t1,0x1f # extract bits 2-6

	bnez $t1, non_int #
		
	andi $t2,$k0,0x00000100  # is bit 8 set?
	bnez $t2, receive
	andi $t2,$t1,0x00000200 # is bit 9 set?
	bnez $t2, transmit
	b iend
	
receive:
	# code to be written by you
	# here follows just an example and should be replaced
	la $t1,TXD
	lw $t2,0($t1)
	li $s1,65
  	sw $s1,0($t2)  
	b iend

transmit:
	# code to be written by you
	# here follows just an example and should be replaced
	la $t1,TXD
	lw $t2,0($t1)
	li $s1,66
  	sw $s1,0($t1)  
	b iend
		
non_int:
	# here we need to increment EPC by 4
	mfc0 $k0,$14
	addiu $k0,$k0,4
	mtc0 $k0,$14
	
iend:
	lw $t1, save_t1
	lw $t2, save_t2
	lw $s1, save_s1
	lw $s2, save_s2
	lw $s3, save_s3
	lw $s4, save_s4
	lw $a0, save_a0
	lw $v0, save_v0
	
	lw $k0, save_at
	move $at,$k0
	mtc0 $zero,$13
	mfc0 $k0,$12
	andi $k0, 0xfffd
	ori $k0,0x0001
	mtc0 $k0,$12
	#return to the address in epc
	eret
