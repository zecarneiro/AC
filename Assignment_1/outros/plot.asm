.data

#Strings para imprimir
bitmap: .word 0x10010000 # EndereÁo inicial do bitmap
request_red: .asciiz "\n Por favor introduza o um n˙mero de 0 a 255 para representar a cor vermelha: \n"
request_green: .asciiz "\n Por favor introduza o um n˙mero de 0 a 255 para representar a cor verde: \n"
request_blue: .asciiz "\n Por favor introduza o um n˙mero de 0 a 255 para representar a cor azul: \n"
filename2: .asciiz "sin2.txt"
filename: .asciiz
filename_request: .asciiz "\n Por favor introduza o nome do ficheiro que contÈm os valores a desenhar: \n"
buffer_str: .asciiz "Buffer: \n" # string auxiliar ao programador
buffer: .space 256 # -> 64*4 ascii -> 3 caracteres + \n = 4
caracteres: .byte 0:3
#------------------------------------------------------------

#vari·veis globais

.align 2
rgb: .word 0:3 #array que armazena valores RGB
.align 2
rgb_clear: .word 255,255,255 #array que armazena valores RGB
.align 2
rgb_axis: .word 0,0,0 #array que armazena valores RGB
.align 2
tabela: .word 0:64

#Strings de erros

error_open_print: .asciiz "Deu Erro no open!\n"
error_read_print: .asciiz "Deu Erro no read!\n"
#------------------------------------------------------------
#------------------------------------------------------------
#------------------------------------------------------------

.text
main:

	jal le_ficheiro_para_buffer

	jal preenche_tabela

	j plot

plot:

	la $s0, tabela
	addi $a1, $zero, 0 # coluna
	
	jal pede_rgb
	
	jal drawAxis
	
	la $a2, rgb
	
	loop_graf:
		la $a0, 0($s0) #load da linha para entrada
		jal setpixel
		addi $a1,$a1,1 #avanÁa coluna
		addi $s0,$s0,4 #avanÁa ponteiro da tabela
		ble $a1, 63, loop_graf
		
	j exit

preenche_tabela:

	la $s0, buffer
	la $s1, tabela
	la $s2, caracteres
	
	addi $t0, $zero, 0
	
	loop_preenche:
		lb $t1, 0($s0)
		sb $t1, 0($s2)
		lb $t1, 1($s0)
		sb $t1, 1($s2)
		lb $t1, 2($s0)
		sb $t1, 2($s2)
		
		la $a0, 0($s2) # mete caracteres como entrada para asciitoint
		jal asciitoint 
					
		sw $v0, 0($s1) #store v0 na tabela
		addi $s1, $s1, 4 #avanÁa pontei para tabela
		
		addi $s0,$s0,3 # avanÁa o buffer 3 casas
		beq $s0,256,plot
		j loop_preenche		
				
	jr $ra					
#----------------------------------------
#-----------3.4.2-----------ASCII TO INT
#----------------------------------------

asciitoint:

	lb $t1, 0($a0) #load do primeiro dÌgito
	lb $t2, 1($a0) #load das dezenas
	lb $t3, 2($a0) #load das unidades
	
	beq $t1, 43, positivo
	beq $t1, 45, negativo
	
	positivo:
		#dezenas
		sub $t0, $t2, 48
		mul $t0, $t0, 10
		add $v0, $v0, $t0
		#unidades
		sub $t0, $t3, 48
		add $v0, $v0, $t0
		
		jr $ra
	
	negativo:
		#dezenas
		sub $t0, $t2, 48
		mul $t0, $t0, 10
		add $v0, $v0, $t0
		#unidades
		sub $t0, $t3, 48
		add $v0, $v0, $t0
		
		mul $v0, $v0, -1
		
		jr $ra
		
#------------------------------------------------------------
#------------------------------------------------------------
	
le_ficheiro_para_buffer:

	addi $t0,$zero,13		# $t0=\r=CR
	addi $t1,$zero,0		# $t1=\0=NULL
	addi $t2,$zero,10		# $t2=\n=LF Nova Linha

	# Imprime pedido de indicaÁ„o sobre execuÁ„o do programa
	li $v0,4
	la $a0, filename_request
	syscall
	
	# Pede nome do ficheiro
	li $v0, 8
	la $a0, filename
	li $a1, 20
	syscall
	
	addi $s0,$zero,13		# $t0=\r=CR
	addi $s1,$zero,0		# $t1=\0=NULL
	addi $s2,$zero,10		# $t2=\n=LF Nova Linha
	
	la $t3, filename		# $t0=endere√ßo do nome do ficheiro
	
	muda_n_null: # muda o ultimo caracter do nome do ficheiro para NULL 
		lb $t4,0($t3)			# $t4=nome_ficheiro[i]
		addi $t3,$t3,1			# incrementa um byte ao endere√É¬ßo $t0
		bne $t4,$t2,muda_n_null		# se nome_ficheiro[i]!=\n entao volta para muda_n_null
		addi $t3,$t3,-1			# como o endere√É¬ßo foi incrementado e a condi√É¬ßao anterior foi verdadeira ent√É¬£o decremento 1
		sb $t1,0($t3)			# nome_ficheiro[i]=\0=null
	
	#opens file
	li   $v0, 13       # system call for open file
	la   $a0, filename  # board file name
	li   $a1, 0        # Open for reading
	li   $a2, 0
	syscall

	# Verifica se nao deu erro
	bltzal $v0, error_open
	move $a0,$v0	# se nao der erro, entao $a0=file descriptor do ficheiro

	#read from file
	li   $v0, 14       # system call for read from file
	la   $a1, buffer   # address of buffer to which to read
	li   $a2, 256      # hardcoded buffer length
	syscall    
	
	# Verifica se nao deu erro
	bltzal $v0, error_read
	move $a1, $a0	# se nao der erro, entao $a0=file descriptor do ficheiro        # read from file
				
	# Close the file 
	li   $v0, 16       # system call for close file
	syscall            # close file

	la $t1, ($a0)
	add $t1,$t1,256 # endereÁo final do ficheiro
	
	jr $ra

#------------------------------------------------------------
#------------------------------------------------------------
	
#-------------------------------------
#-----------3.4.3-----------DrawAxis
#-------------------------------------
				
drawAxis:

	la $a2, rgb_clear # load rgb branco
	
	jal clear #limpar pixel de erro e pÙr ecr„ a branco
	
	li $a0, 31
	addi $a1, $zero, 0
	la $a2, rgb_axis # load rgb preto
	#DrawAxis
	looper_DA:
		jal setpixel
		addi $a1, $a1, 1
		ble $a1, 63, looper_DA

	jr $ra

#-------------------------------
#----------------------PEDE RGB
#-------------------------------

pede_rgb:
	la $t0,rgb

	# Pede Vermelho
	li $v0, 4
	la $a0, request_red
	syscall
	
	# Pede o numero vermelho
	li $v0,5
	syscall

	sw $v0, 0($t0) #store da cor vermelha
	
	# Pede Verde
	li $v0, 4
	la $a0, request_green
	syscall
	
	# Pede o numero verde
	li $v0,5
	syscall
	
	sw $v0, 4($t0)  #store da cor verde

	# Pede Azul
	li $v0, 4
	la $a0, request_blue
	syscall
	
	# Pede o numero azul
	li $v0,5
	syscall
	
	sw $v0, 8($t0)  #store da cor azul
	
	#la $a2,rgb #o endere√ßo do rgb √© passado para a2
	
	jr $ra
	
#---------------------------------
#-----------3.4.4-----------CLEAR
#---------------------------------

clear:

	#jal pede_rgb

	la, $t0, 0($a2) #load da cor a usar no clear para o registo $t0
	la, $t6, bitmap #load do endereÁo do bitmap para $t6
	
	#Calculo RGB
	lw $t1, 0($t0) #load red	
	add $t2, $t2, $t1
	sll $t2, $t2, 8 #shift left 8 casas
	lw $t1, 4($t0) #load green
	add $t2, $t2, $t1
	sll $t2, $t2, 8#shift left 8 casas
	lw $t1, 8($t0) #load blue
	add $t2, $t2, $t1 #cor final

	#Calculo do tamanho do bitmap
	add $t3,$t3,64
	mul $t3,$t3,$t3
	mul $t3,$t3,4
	#Calculo do endereÁo final do bit map
	add $t4,$t4,$t6
	add $t4,$t4,$t3 # -> endereÁo final

	loop:
		sw $t2,0($t6) #armazena a cor no pixel designado por 0($t6)
		add $t6,$t6,4 #desloca o ponteiro que aponta para os pixeis individuais do bitmap
		beq $t6,$t4,exit_clear # verifica se todos os pixeis est„o preenchidos
		j loop
	
	exit_clear:
			
		jr $ra	
#-------------------------------------
#-----------3.4.3-----------SET PIXEL
#-------------------------------------
				
setpixel:
	
	la $t7,bitmap

	addi $s0,$a0,0 #load da linha
	addi $s1,$a1,0 #load da coluna	
	#la $s0, 0($a0) #load da linha
	#la $s1, 0($a1) #load da coluna	
	#jal pede_rgb
	
	la $t0, 0($a2) #load do RGB
	
	#Calculo da posicao
	#linha
	mul $s0, $s0, 64
	mul $s0, $s0, 4
	add $t7,$t7,$s0
	#coluna
	mul $s1, $s1, 4
	#final position
	add $t7,$t7,$s1 #-> pixel position 
	
	#Calculo RGB
	lw $s2, 0($a2) #load red	
	add $t0, $t0, $s2
	sll $t0, $t0, 8 #shift left
	lw $s2, 4($a2) #load green	
	add $t0, $t0, $s2
	sll $t0, $t0, 8 #shift left
	lw $s2, 8($a2) #load blue	
	add $t0, $t0, $s2

	#store da cor
	sw $t0, 0($t7)
	
	jr $ra		
		
#------------------------------------------------------------
#------------------------------------------------------------		
		
#------------------------------------------------------------


#---------------------------------
#----------------------ERROR OPEN
#---------------------------------

error_open:

	# Imprime pedido de indica√ß√£o sobre execu√ß√£o do programa
	li $v0, 4
	la $a0, error_open_print
	syscall
	
	jr $ra


#------------------------------------------------------------
#------------------------------------------------------------

#---------------------------------
#----------------------ERROR READ
#---------------------------------

error_read:

	# Imprime pedido de indica√ß√£o sobre execu√ß√£o do programa
	li $v0, 4
	la $a0, error_read_print
	syscall

	jr $ra
		
#=========================== Fim ===========================

exit:

	li $v0, 10
	syscall	
