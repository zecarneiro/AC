# Exercicio 3.5

.data
bitmap: .space 270000
filename: .asciiz
error_open: .asciiz "Deu Erro no open!\n"
error_read: .asciiz "Deu Erro no read!\n"
filename_request: .asciiz "\n Por favor introduza o nome do ficheiro que contém os valores a desenhar: \n"
	
buffer: .space 1
buffer_str: .asciiz "\nBuffer: " # string auxiliar ao programador
cor_str: .asciiz "\n Cor:" # string auxiliar ao programador
linha: .byte 0:2
coluna: .byte 0:2
.text

#t0 - usada para o bitmap
#t1 - Contador que verifica se estamos na 2� linha -> para leitura do tamanho
#t2 - Contador que verifica se estamos na coluna m�xima
#t3 - Vari�vel para verificar se j� se passou o espa�o na 2� linha
#t4 - load do buffer para c�lculo da cor
#t5 - cont�m a cor final
#t6 - File Descriptor
#t7 - Vari�vel onde se mete o buffer
#s0 - linha: .byte 0:2
#s1 - coluna: .byte 0:2
#s2 - caracter das variaveis linha/coluna
#s3 - inteiro convertido
#s4 - inteiro linha
#s5 - inteiro coluna
#s6 - linha maxima
#s7 - coluna maxima


main:

	la $t0, bitmap # load do endere�o de bitmap
	la $s0, linha # load do do endere�o de linha
	la $s1, coluna # load do do endere�o de coluna
	
	#\0 = 0
	# \n = 10
	#space = 32
	addi $t1,$zero,0  #Inicializa��o de contador
	addi $t2,$zero,0  #Inicializa��o de contador
	addi $t3,$zero,0  #Inicializa��o de verificador
	
	# Imprime pedido de indicação sobre execução do programa
	li $v0,4
	la $a0, filename_request
	syscall
	
	# Pede nome do ficheiro
	li $v0, 8
	la $a0, filename
	li $a1, 200 # o nome do ficheiro só pode ter um tamanho maximo de 200 bytes
	syscall
	
	la $t3, filename		# $t0=endereÃ§o do nome do ficheiro
	
	muda_n_null: # muda o ultimo caracter do nome do ficheiro para NULL 
		lb $t4,0($t3)			# $t4=nome_ficheiro[i]
		addi $t3,$t3,1			# incrementa um byte ao endereÃ§o $t0
		bne $t4,$t2,muda_n_null		# se nome_ficheiro[i]!=\n entao volta para muda_n_null
		addi $t3,$t3,-1			# como o endereço foi incrementado e a condição anterior foi verdadeira então decremento 1
		sb $t1,0($t3)			# nome_ficheiro[i]=\0=null
	
	j le_para_buffer
		
le_para_buffer:		
	
	#opens file
	li   $v0, 13       # system call for open file
	la   $a0, filename  # board file name
	li   $a1, 0        # Open for reading
	li   $a2, 0
	syscall

	# Verifica se nao deu erro
	bltz $v0, error1
	move $a0,$v0	# se nao der erro, entao $a0=file descriptor do ficheiro

	add $t6,$a0,$zero #store do file descriptor
	
	salta_header:
		#l� caracter
		addi   $v0,$zero, 14       # system call for read from file
		la   $a1, buffer   # address of buffer to which to read
		addi   $a2,$zero, 1      # hardcoded buffer length
		syscall 
		
		lb $t7, buffer
		beq $t7, 10, conta
		beq $t1, 1, le_linha_coluna
		j salta_header
	conta: #Incrementa contador de /n
		addi $t1, $t1, 1
		beq $t1, 3, converte_int
		j salta_header

	le_linha_coluna: #L� tamanho da imagem
		beq $t7, 32, skip
		bge $t3, 1, le_coluna
		sb $t7, 0($s0)
		addi $s0,$s0,1
		j salta_header
		skip:
			addi $t3, $t3, 1
			j salta_header
		le_coluna:
			sb $t7, 0($s1)
			addi $s1,$s1,1
			j salta_header
			
converte_int:
	la $s0, linha # load do do endere�o de linha
	la $s1, coluna # load do do endere�o de coluna
	
	# Converte para inteiro
	# Linha
	lb $s2,0($s0)
	addi $s2,$s2,-48
	mul $s3,$s2,10
	addi $s0,$s0,1
	lb $s2,0($s0)
	addi $s2,$s2,-48
	add $s3,$s3,$s2
	addi $s3,$s4,0
	
	# Coluna
	addi $s4,$s4,4
	
	lb $s2,0($s1)
	addi $s2,$s2,-48
	mul $s3,$s2,10
	addi $s1,$s1,1
	lb $s2,0($s1)
	addi $s2,$s2,-48
	add $s3,$s3,$s2
	addi $s3,$s5,0
	
	# Coluna maxima
	mul $s7,$s5,4
	add $s7,$t0,$s7
	add $s7,$s7,-4
	
	# linha maxima
	addi $s6,$s7,4

escreve:		
		add $a0, $zero, $t6
		#l� caracter
		addi   $v0,$zero, 14       # system call for read from file
		la   $a1, buffer   # address of buffer to which to read
		addi   $a2,$zero, 1      # hardcoded buffer length
		syscall 
		
		# Verifica se nao deu erro
		bltz $v0, error2
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
	
	verifica_paragrafo: #verifica se ocorre um \n, se sim, salta para o pr�ximo byte
		lb $t7, buffer
		beq $t7, 10, escreve
	
		
	loop_escreve:

		#Calculo GRAYSCALE
		lb $t4, buffer #Load do buffer
		and $t4, $t4, 0x000000ff # endere�o da cor blue	
		
		sll $t5, $t4, 8 #shift left 8 casas
		or $t5, $t5, $t4
		
		sll $t5, $t5, 8 #shift left 8 casas
		
		or $t5, $t5, $t4 #cor final

		#-------------

		# Imprime cor_str
		li $v0, 4
		la $a0, cor_str
		syscall

		# Imprime cor
		li $v0, 1
		add $a0, $zero, $t5
		syscall
			
		beq $t0, $s7, muda_linha #Se o contador for igual a 63, muda a linha -> mudar 63 para o registo que contiver a vari�vel com tamanho coluna
				
		sw $t5,0($t0)	# Pixel introduzido no bitmap
		
		addi $t0, $t0, 4 #incrementa��o da posi��o do bitmap
				
		j escreve
	
	muda_linha:
		addi $t2, $zero, 0  #RESET do contador de colunas
		addi $t0,$s6,0
		
		# Coluna maxima
		mul $s7,$s5,4
		add $s7,$s4,$s7
		add $s7,$s7,-4
		
		# linha maxima
		addi $s6,$s7,4
		
		sw $t5, 0($t0) # Pixel introduzido no bitmap
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
		
	
		
		
