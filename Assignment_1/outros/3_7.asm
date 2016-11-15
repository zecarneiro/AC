#Crie um programa que pergunte ao utilizador se quer: 1) limpar o ecr„
#ou 2) desenhar gr·fico. No primeiro caso deve pedir os valores R, G,B
#e limpar. No segundo deve pedir o nome de um ficheiro que contÈm
#64 linhas com valores que devem ser usadas para desenhar o gr·fico
#e deve tambÈm pedir os valores R,G,B da cor que o gr·fico deve ter.
#Chamadas sucessivas desta opÁ„o ir· sobrepor v·rios gr·ficos. Nota:
#no ficheiro auxiliar poder· encontrar v·rios ficheiros coma extens„o
#.txt que dever· usar para testar

#------------------------------------------------------------

.data

#Strings para imprimir

main_request: .asciiz "Pretende limpar o bitmap - 1 | Ou | Desenhar Gr·fico - 2 \n"
filename_request: .asciiz "\n Por favor introduza o nome do ficheiro que contÈm os valores a desenhar: \n"
request_red: .asciiz "\n Por favor introduza o um n˙mero de 0 a 255 para representar a cor vermelha: \n"
request_green: .asciiz "\n Por favor introduza o um n˙mero de 0 a 255 para representar a cor verde: \n"
request_blue: .asciiz "\n Por favor introduza o um n˙mero de 0 a 255 para representar a cor azul: \n"
#buffer_str: .asciiz "Buffer: \n" # string auxiliar ao programador

#------------------------------------------------------------

#vari·veis globais

bitmap: .word 0x10010000 # EndereÁo inicial do bitmap
caracteres: .byte 0:3 #array que armazena caracteres lidos no buffer
rgb: .word 0:3 #array que armazena valores RGB
filename: .asciiz
buffer: .space 256 # -> 64*4 ascii -> 3 caracteres + \n = 4
tabela_int: .word 0:64

#------------------------------------------------------------

#Strings de erros

error_open_print: .asciiz "Deu Erro no open!\n"
error_read_print: .asciiz "Deu Erro no read!\n"


#------------------------------------------------------------
#------------------------------------------------------------
#------------------------------------------------------------

#----------------------------MAIN----------------------------

.text
main:

	addi $s3,$zero,10		# $s3=\n=LF Nova Linha
	addi $s0,$zero,13		# $s0=\r=CR
	addi $s1,$zero,0		# $s1=\0=NULL

	# Imprime pedido de indicaÁ„o sobre execuÁ„o do programa
	li $v0, 4
	la $a0, main_request
	syscall
	
	# Pede o numero
	li $v0,5
	syscall
						
	beq $v0,1,clear
	
	beq $v0,2,le_ficheiro
	
	#j exit

#-------------------------------------------------------------
#---------------------------FUN«’ES---------------------------
#-------------------------------------------------------------

#----------------------------------------
#-----------3.4.2-----------ASCII TO INT
#----------------------------------------

# 3.4.2 - Fun√ß√£o recebe um array de 3 caracteres e devolve um inteiro
# Irei usar os registos
# t0 = array de caracter, ou seja, o endere√ßo do array
# t1 = caracter qualquer do array
# t2 = 10
# t3 = \0 = NULL
# t4 = Inteiro pretendido
# t5 = 1 ou -1, ou seja, √© o sinal do inteiro
# t6 = caracter que j√° foi convertido para inteiro
asciitoint:
	addi $t0,$a0,0			# t0 = cval, ou seja, t3=argumento
	lb $t1,0($t0)			# t1 = cval[i], ou seja, t1 = caracter
	
	addi $t2,$zero,10		# t2 = 10, Como o numero s√≥ tem 2 casas decimais, ent√£o mult o 1¬∫ por 10
	addi $t3,$zero,0		# $t3=\0=NULL, todas as strings/array de caracteres terminam em \0
	addi $t4,$zero,0		# Como tenho que fazer uma soma,ent√£o come√ßo com zero

	beq $t1,45,num_negativo		# Se o primeiro caracter for (-), vai para num_negativo
	
	# Se o primeiro caracter for (+), continua
	addi $t5,$zero,1		# t5 = 1, e vai ser usado para multiplicar com o inteiro convertido
	addi $t0,$t0,1			# √â incrementado o endere√ßo para o proximo caracter
	lb $t1,0($t0)			# t1 = cval[i], ou seja, t1 = caracter
	
	loop_toint:	
		addi $t6,$t1,-48	# Em ASCII t1 = 2 = 50, ent√£o para ser o numero 2 sub t6 = 50-48=2, porque 48 = 0 em ascii
		mul $t6,$t6,$t2		# como s√£o 2 caracteres ent√£o para contruir um int tenho que t6=caracter * 10
		add $t4,$t4,$t6		# depois mult √© s√≥ adicionar o numero pelo proximo, ou seja,ex:1000+100+10+1	
		div $t2,$t2,10		# tenho que baixar sempre o ex: 1000/10 -> 100/10 -> 10/10 ->1
	
		addi $t0,$t0,1		# √â incrementado o endere√ßo para o proximo caracter
		lb $t1,0($t0)		# t1 = cval[i], ou seja, t1 = caracter
		bne $t1,$t3,loop_toint	# Se for cval[i]=\0 quer dizer que j√° leu os 3 caracteres, ou seja o inteiro finalizado
	
		mul $t4,$t4,$t5		# O inteiro √© convertido para positivo ou negativo
	
		add $v0,$t4,$zero	# Retorna o inteiro desejado
		jr $ra

	# Se o numero for negativo	 
	num_negativo:
		addi $t5,$zero,-1	# t5 = -1, e vai ser usado para multiplicar com o inteiro convertido
		addi $t0,$t0,1		# √â incrementado o endere√ßo para o proximo caracter
		lb $t1,0($t0)		# t4 = cval[i], ou seja, t4 = caracter
		j loop_toint		# continuo a conver√ß√£o para inteiro
		
#------------------------------------------------------------
#------------------------------------------------------------

#-------------------------------------
#-----------3.4.3-----------SET PIXEL
#-------------------------------------
				
setpixel:

	la,$t6,bitmap

	lw $t0, 0($a0)
	lw $t1, 0($a1)
	la $t2, ($a2)

	#AssunÔøΩÔøΩo, falar com prof
	#-----------
	#sub $t0, $t0, 1
	#sub $t1, $t1, 1
	#-----------

	#Calculo da posiÔøΩÔøΩo
	#linha
	mul $t0, $t0, 64
	mul $t0, $t0, 4
	add $t6,$t6,$t0
	#coluna
	mul $t1, $t1, 4
	#posiÔøΩÔøΩo final
	add $t6,$t6,$t1
	
	#Calculo RGB
	lw $t4, 0($t2) #load red	
	add $t5, $t5, $t4
	sll $t5, $t5, 8 #shift left
	lw $t4, 4($t2) #load green
	add $t5, $t5, $t4
	sll $t5, $t5, 8 #shift left
	lw $t4, 8($t2) #load blue
	add $t5, $t5, $t4 #cor final
	
	#store da cor
	sw $t5,0($t6)
	
#------------------------------------------------------------
#------------------------------------------------------------	

#---------------------------------
#-----------3.4.4-----------CLEAR
#---------------------------------

clear:

	jal pede_rgb

	la, $t0, ($a2) #load da cor a usar no clear para o registo $t0
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
		beq $t6,$t4,exit # verifica se todos os pixeis est„o preenchidos
		j loop
	
#------------------------------------------------------------
#------------------------------------------------------------

#-------------------------------------
#-----------3.4.5-----------DRAW AXIS
#-------------------------------------

# 3.4.5 - Fun√ß√£o que desenha uma linha preta no meio do ecr√£
# s0 = endere√ßo de rgb
# s1 = linha do meio
# s2 = coluna
# s3 = cor preta r=g=b=0
drawaxis:
	la $s0,rgb			# s0 = endere√ßo da tabela rgb
	
	addi $s1,$zero,64		# s1 = Numero total de linhas = 64
	div $s1,$s1,2			# s1 = metade das linhas
	addi $s1,$s1,-1			# Como as linhas vai de 0 a 63 ent√£o, (total/2)-1 = centro
	
	addi $s2,$zero,0		# s2 = colunas
	
	addi $s3,$zero,0		# Como a cor preta √© composta por r=0,g=0,b=0, ent√£o s3 = 0
	
	sw $s3,0($s0)			# R = 0
	sw $s3,4($s0)			# G = 0
	sw $s3,8($s0)			# B = 0
	
	addi $a2,$s0,0			# a2 = endere√ßo rgb
	addi $a0,$s1,0			# a0 = linha
	
	loop_draw:
		addi $a1,$s2,0		# a1 = coluna
		jal setpixel		# vai a setpixel e introduz o pixel na posi√ß√£o desejada
		addi $s2,$s2,1		# √© incrementada para a proxima coluna
		ble $s2,63,loop_draw	# Enquanto for menor que as colunas todas, repete
	
	jr $ra



#------------------------------------------------------------
#------------------------------------------------------------

#--------------------------------
#-----------3.4.6-----------PLOT
#--------------------------------

# 3.4.6 - Fun√ß√£o que desenha o gr√°fico
# s0 = endere√ßo do rgb
# s1 = endere√ßo da tabela de valores
# s2 = linha do meio
# s3 = inteiro da tebela
# s4 = coluna
# s5 = inteiro para enviar para setpixel
plot:
	jal drawaxis

	addi $s0,$a2,0			# s0 = a1 = rgb
	addi $s1,$a0,0			# s1 = a0 = tabela com os valores
	
	addi $s2,$zero,64		# s2 = Numero total de linhas = 64
	div $s2,$s2,2			# s2 = metade das linhas
	addi $s2,$s2,-1			# Como as linhas vai de 0 a 63 ent√£o, (total/2)-1 = centro
	
	addi $s4,$zero,0		# s4 = colunas, que come√ßa em zero at√© 63
	
	loop_plot:
		lw $s3,0($s1)		# s3 = tab_valore[i] = ponto do grafico
		
		# Como os valores negativos est√£o depois da linha do meio, e os valores v√™m com o sinal negativo
		# e tenho de adicionar a linha do meio mais a linha que est√° o ponto ent√£o multiplico por -1 os valores da tabela.
		# Os valores positivos est√£o antes da linha do meio, ent√£o tenho que subtrair a linha do meio para o ponto
		# que deve ser desenhado no grafico. Por isso, multiplico por -1. 
		mul $s3,$s3,-1		
		
		add $s5,$s2,$s3		# s5 = linha do meio + linha que deve desenhar o ponto
		addi $a0,$s5,0		# a0 = linha
		addi $a1,$s4,0		# a1 = coluna
		addi $s1,$s1,4		# Proximo inteiro da tabela
		jal setpixel		# vai a setpixel e introduz o pixel na posi√ß√£o desejada
		addi $s4,$s4,1		# √© incrementada para a proxima coluna
		ble $s4,15,loop_plot	# Enquanto for menor que as colunas todas, repete
		
#------------------------------------------------------------
#------------------------------------------------------------

#---------------------------------------
#-----------3.4.7-----------LE FICHEIRO
#---------------------------------------

le_ficheiro:

	jal pede_rgb

	# Imprime pedido de indicaÁ„o sobre execuÁ„o do programa
	li $v0,4
	la $a0, filename_request
	syscall
	
	# Pede nome do ficheiro
	li $v0, 8
	la $a0, filename
	li $a1, 20
	syscall
	
	la $t0, filename		# $t0=endere√ßo do nome do ficheiro
	
	jal muda_n_null			# Vai a leficheiro e guarda o endere√ßo em ra


	#opens file
	li   $v0, 13       # system call for open file
	la   $a0, filename  # board file name
	li   $a1, 0        # Open for reading
	li   $a2, 0
	syscall

	# Verifica se nao deu erro
	bltz $v0, error_open
	move $a0,$v0	# se nao der erro, entao $a0=file descriptor do ficheiro

	#read from file
	li   $v0, 14       # system call for read from file
	la   $a1, buffer   # address of buffer to which to read
	li   $a2, 256      # hardcoded buffer length
	syscall    
	
	# Verifica se nao deu erro
	bltz $v0, error_read
	move $a1, $a0	# se nao der erro, entao $a0=file descriptor do ficheiro        # read from file
				
	# Close the file 
	li   $v0, 16       # system call for close file
	syscall            # close file

	la $t1, ($a0)
	add $t1,$t1,256 # endereÁo final do ficheiro
	
	j converte_para_array	
	
#------------------------------------------------------------
#------------------------------------------------------------

#-------------------------------------------------------------
#----------------------FUN«’ES AUXILIARES---------------------
#-------------------------------------------------------------

#---------------------------------------------------
#----------------------CONVERTE FICHEIRO PARA ARRAY
#---------------------------------------------------

converte_para_array:

	la $t0, ($a0)
	la $t1, tabela_int
	la $t2, tabela_int
	
	outside_looper:
	
		jal caracteres_para_int #armazena 3 caracteres numa array
	
		la $a0, caracteres
	
		looper:		
			jal asciitoint #converte array para int

			sw $v0, 0($t1)
			addi $t1, $t1, 4
		
			addi $t0,$t0,1
		
			la $a0, ($t2)
			lw $t3, 0($t0)
			beq $t3,$s0, exit
			j outside_looper
			
	
	jr $ra
	
#------------------------------------------------------------
#------------------------------------------------------------

#------------------------------------------
#----------------------CARACTERES PARA INT
#------------------------------------------


caracteres_para_int:

	addi $t0, $a0, 0
	la $t1,caracteres
	la $v0,caracteres
	
	la $t7, ($ra)
	#addi $t7,$t7,1
	
	loop_carac:
		lb $t2, 0($t0)	
		sb $t2, 0($t1)
		addi $t0, $t0, 1
		addi $t1, $t1, 1
		lb $t3, 0($t0)
		beq $t3,$s3, looper
		beq $t3,$s0, looper
		j loop_carac
		
	jr $t7
		
#------------------------------------------------------------
#------------------------------------------------------------

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
	
	la $a2,rgb #o endere√ßo do rgb √© passado para a2
	
	jr $ra
	
#------------------------------------------------------------
#------------------------------------------------------------

#----------------------------------
#----------------------MUDA N NULL
#----------------------------------

muda_n_null: # muda o ultimo caracter do nome do ficheiro para NULL 
	lb $t1,0($t0)			# $t1=nome_ficheiro[i]
	addi $t0,$t0,1			# incrementa um byte ao endere√ßo $t0
	bne $t1,$s3,muda_n_null		# se nome_ficheiro[i]!=\n entao volta para muda_n_null
	addi $t0,$t0,-1			# como o endere√ßo foi incrementado e a condi√ßao anterior foi verdadeira ent√£o decremento 1
	sb $s1,0($t0)			# nome_ficheiro[i]=\0=null
	jr $ra				# Volta para o main onde estava quando chamou muda_n_null
		
#------------------------------------------------------------
#------------------------------------------------------------
						

#---------------------------------
#----------------------ERROR OPEN
#---------------------------------

error_open:

	# Imprime pedido de indica√ß√£o sobre execu√ß√£o do programa
	li $v0, 4
	la $a0, error_read_print
	syscall


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

#------------------------------------------------------------
#------------------------------------------------------------

#---------------------------
#----------------------EXIT
#---------------------------

exit:

	li $v0, 10
	syscall
