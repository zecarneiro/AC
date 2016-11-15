# Exercicio 3.5

.data
bitmap: .space 270000
filename: .asciiz "ht.pgm"
error_open: .asciiz "Deu Erro no open!\n"
error_read: .asciiz "Deu Erro no read!\n"
	
buffer: .space 1
buffer_str: .asciiz "\nBuffer: " # string auxiliar ao programador
cor_str: .asciiz "\n Cor:" # string auxiliar ao programador
linha: .byte 0:2
coluna: .byte 0:2
.text

main:

	la $t9, bitmap
	la $s6, linha
	la $s7, coluna
	
	#\0 = 0
	# \n = 10
	#space = 32
	addi $t4,$zero,0  #contador1
	addi $t6,$zero,0  #contador3
	addi $s0,$zero,0  #trigger para store da coluna
	
	jal le_para_buffer

	jal escreve
	
	j exit
		
le_para_buffer:		
	
	#opens file
	li   $v0, 13       # system call for open file
	la   $a0, filename  # board file name
	li   $a1, 0        # Open for reading
	li   $a2, 0
	syscall

	# Verifica se nao deu erro
	bltz $v0, error_open
	move $a0,$v0	# se nao der erro, entao $a0=file descriptor do ficheiro

	add $s5,$a0,$zero #store do file descriptor
	

	
	salta_header:
		#l� caracter
		addi   $v0,$zero, 14       # system call for read from file
		la   $a1, buffer   # address of buffer to which to read
		addi   $a2,$zero, 1      # hardcoded buffer length
		syscall 
		
		lb $t7, buffer
		beq $t7, 10, conta
		beq $t4, 1, le_linha_coluna
		j salta_header
	conta:
		addi $t4, $t4, 1
		beq $t4, 3, escreve
		j salta_header

	le_linha_coluna:
		beq $t7, 32, skip
		bge $s0, 1, le_coluna
		sb $t7, 0($s6)
		addi $s6,$s6,1
		j salta_header
		skip:
			addi $s0, $s0, 1
			j salta_header
		le_coluna:
			sb $t7, 0($s7)
			addi $s7,$s7,1
			j salta_header
	jr $ra

escreve:		
		add $a0, $zero, $s5
		#l� caracter
		addi   $v0,$zero, 14       # system call for read from file
		la   $a1, buffer   # address of buffer to which to read
		addi   $a2,$zero, 1      # hardcoded buffer length
		syscall 
		
		# Verifica se nao deu erro
		bltz $v0, error_read
		beq $v0, 0, exit_func
		#move $a1, $a0	# se nao der erro, entao $a0=file descriptor do ficheiro        # read from file
		
		# Imprime buffer_str
		li $v0, 4
		la $a0, buffer_str
		syscall

		# Imprime buffer
		li $v0, 4
		la $a0, buffer
		syscall
	
	verifica_paragrafo:
		lb $t7, buffer
		beq $t7, 10, escreve
	
		
	loop_escreve:

		#Calculo RGB
		lb $s1, buffer #load red
		and $s1, $s1, 0x000000ff # endere�o da cor blue	
		#add $s2, $s2, $s1
		sll $s2, $s2, 8 #shift left 8 casas
		or $s2, $s2, $s1
		#add $s2, $s2, $s1
		sll $s2, $s2, 8 #shift left 8 casas
		#add $s2, $s2, $s1 #cor final
		or $s2, $s2, $s1

		# Imprime cor_str
		li $v0, 4
		la $a0, cor_str
		syscall

		# Imprime cor
		li $v0, 1
		add $a0, $zero, $s2
		syscall
			
		beq $t6, 63, muda_linha
			
		#addi $t6, $t6, 1
				
		sw $s2,0($t9)		# Pixel introduzido no bitmap
		
		#addi $s0, $s0, 1
		addi $t9, $t9, 4
				
		j escreve
	
	muda_linha:
		addi $t6, $zero, 0  #contador3
		addi $t9, $t9, 4
		sw $s2, 0($t9)
		j escreve
				
	exit_func:
		
		# Close the file 
		li   $v0, 16       # system call for close file
		syscall            # close file

		j exit			
		
#------------------------------------------------------------
#------------------------------------------------------------

error1:

	# Imprime pedido de indicação sobre execução do programa
	li $v0, 4
	la $a0, error_open
	syscall
	
#------------------------------------------------------------
#------------------------------------------------------------

error2:

	# Imprime pedido de indicação sobre execução do programa
	li $v0, 4
	la $a0, error_read
	syscall

#------------------------------------------------------------
#------------------------------------------------------------

exit:


	li $v0,10
	syscall
		
	
		
		
