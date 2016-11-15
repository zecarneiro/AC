# /home/jose/Documentos/AC/Assignment_1/cos2.txt

.data
bitmap: .space 16384				# bitmap = espaço reservado para pixeis, ou seja (4*64)*64
rgb: .space 12					# rgb = Endereço para 3 inteiros
tabela_valores: .word 0:64			# tabela_valores = Tabela[64]
cval: .byte 0:3					# variavel que contém os numeros em ascii, ou seja, cval[3]

# Mensagens de informaçoes e erro
main_request: .asciiz "Pretende limpar o bitmap - 1 | Ou | Desenhar Gráfico - 2 \n"
filename_request: .asciiz "\n Por favor introduza o nome do ficheiro que contém os valores a desenhar: \n"
request_red: .asciiz "\n Por favor introduza o um número de 0 a 255 para representar a cor vermelha: \n"
request_green: .asciiz "\n Por favor introduza o um número de 0 a 255 para representar a cor verde: \n"
request_blue: .asciiz "\n Por favor introduza o um número de 0 a 255 para representar a cor azul: \n"
error_open: .asciiz "Deu Erro no open!\n"
error_read: .asciiz "Deu Erro no read!\n"

filename: .asciiz
buffer: .space 2700 # armazena no máximo 2700 bytes

.text
main:
	# Imprime pedido de indicação sobre execução do programa
	li $v0, 4
	la $a0, main_request
	syscall
	
	# Pede o numero
	li $v0,5
	syscall
	
	# Se o numero introduzido for 1, vai para clear					
	beq $v0,1,clear
	
	# Se o numero introduzido for 2, vai para le_ficheiro
	beq $v0,2,le_ficheiro

#=========================== ASCIITOINT ===========================
# 3.4.2 - Função recebe um array de 3 caracteres e devolve um inteiro
# Irei usar os registos
# t0 = array de caracter, ou seja, o endereço do array
# t1 = caracter qualquer do array
# t2 = 10
# t4 = Inteiro pretendido
# t5 = 1 ou -1, ou seja, é o sinal do inteiro
# t6 = caracter que já foi convertido para inteiro
# t7 = numero de caracter transformado
asciitoint:
	addi $t0,$a0,0			# t0 = cval, ou seja, t3=argumento
	lb $t1,0($t0)			# t1 = cval[i], ou seja, t1 = caracter
	
	addi $t2,$zero,10		# t2 = 10, Como o numero só tem 2 casas decimais, então mult o 1º por 10
	addi $t3,$zero,0		# $t3=\0=NULL, todas as strings/array de caracteres terminam em \0
	addi $t4,$zero,0		# Como tenho que fazer uma soma,então começo com zero
	addi $t7,$zero,0		# Como tenho que fazer uma soma,então começo com zero

	beq $t1,45,num_negativo		# Se o primeiro caracter for (-), vai para num_negativo
	
	# Se o primeiro caracter for (+), continua
	addi $t5,$zero,1		# t5 = 1, e vai ser usado para multiplicar com o inteiro convertido
	addi $t0,$t0,1			# É incrementado o endereço para o proximo caracter
	lb $t1,0($t0)			# t1 = cval[i], ou seja, t1 = caracter
	
	loop_toint:	
		addi $t6,$t1,-48	# Em ASCII t1 = 2 = 50, então para ser o numero 2 sub t6 = 50-48=2, porque 48 = 0 em ascii
		mul $t6,$t6,$t2		# como são 2 caracteres então para contruir um int tenho que t6=caracter * 10
		add $t4,$t4,$t6		# depois mult é só adicionar o numero pelo proximo, ou seja,ex:1000+100+10+1	
		div $t2,$t2,10		# tenho que baixar sempre o ex: 1000/10 -> 100/10 -> 10/10 ->1
	
		addi $t0,$t0,1		# É incrementado o endereço para o proximo caracter
		lb $t1,0($t0)		# t1 = cval[i], ou seja, t1 = caracter
		addi $t7,$t7,1
		blt $t7,2,loop_toint	# Se t7 < 2 quer dizer que já leu os 3 caracteres, ou seja o inteiro finalizado
	
		mul $t4,$t4,$t5		# O inteiro é convertido para positivo ou negativo
	
		add $v0,$t4,$zero	# Retorna o inteiro desejado
		jr $ra

	# Se o numero for negativo	 
	num_negativo:
		addi $t5,$zero,-1	# t5 = -1, e vai ser usado para multiplicar com o inteiro convertido
		addi $t0,$t0,1		# É incrementado o endereço para o proximo caracter
		lb $t1,0($t0)		# t4 = cval[i], ou seja, t4 = caracter
		j loop_toint		# continuo a converção para inteiro
	
#=========================== Fim ===========================

#=========================== SETPIXEL ===========================
# 3.4.3 - Função recebe linha,coluna,red,green,blue) e introduz o pixel
# t0 = endereço do bitmap
# t1 = a0 = linha
# t2 = a1 = coluna
# t3 = a2 = endereço da tabela rgb
# t4 = 3, que é o numero de iterações a ser feita para lêr restantes dos argumentos de a0, ou seja, RGB
# t5 = Cor criada
# t6 = Calculos auxiliares
# t7 = cada valor de cada cor da tabela = rgb[i]
setpixel:
	la,$t0,bitmap			# t0 = endereço do bitmap
	
	addi $t1,$a0,0			# t1 = a0 = linha
	addi $t2,$a1,0			# t2 = a1 = coluna
	addi $t3,$a2,0			# t3 = a2 = endereço que contém os inteiros rgb
	
	addi $t6,$zero,64		# Uma linha tem 64 pixeis, que vai de 0 a 63 
	mul $t6,$t6,4			# Resultado de uma linha completa
	
	mul $t1,$t1,$t6			# Linha desejada
	add $t0,$t0,$t1			# O endereço da Linha pretendida, calculada
	
	mul $t2,$t2,4			# Coluna desejada			.
	add $t0,$t0,$t2			# O endereço da coluna pretendida, calculada
	
	# Cor	
	addi $t4,$zero,2		# t4 = numero de argumentos(RGB) que faltam para condição do loop	
	addi $t5,$zero,0
	
	loop_cor:
		lw $t7,0($t3)		# Recebe os argumento da seguinte ordem (R,G,B)
		add $t5,$t5,$t7		# Vai adicionando os argumentos
	
		# Se o primeiro for DEC(255) e BIN(11111111), entao em HEX(0x000000ff) ao fazer sll de 8 bits,
		# fica BIN(11111111 00000000) que em HEX(0x0000ff00) 		
		sll $t5,$t5,8
			
		addi $t3,$t3,4		# É incrementada para o proximo argumento
		addi $t4,$t4,-1		# t4 = t4-1
		bgtz $t4,loop_cor	# Sai, assim que t4 = 0
	
	lw $t7,0($t3)			# Recebe os argumento da seguinte ordem (R,G,B), ultimo que faltava	
	add $t5,$t5,$t7			# Vai adicionando os argumentos	
	sw $t5,0($t0)			# Pixel introduzido no bitmap
		
	jr $ra
#=========================== Fim ===========================	

#=========================== CLEAR ===========================
# 3.4.4 - Função recebe os valores rgb e preenche todos os pixeis
#$a1 -> argumento com cor
#$t0 -> tabela com cor
#$t6 -> bitmap
#$t1 -> cor temporária
#$t2 -> cor final
#$t3 -> variável auxiliar para calcular o endereço final do bitmap
#$t4 -> armazena endereço final do bitmap
clear:
	jal pede_rgb
	
	addi $t0,$a2,0 #load da cor a usar no clear para o registo $t0
	la, $t6, bitmap #load do endereço do bitmap para $t6
	
	addi $t2,$zero,0
	addi $t3,$zero,0
	
	#Calculo RGB
	lw $t1, 0($t0) #load red	
	add $t2, $t2, $t1
	sll $t2, $t2, 8 #shift left 8 casas
	
	lw $t1, 4($t0) #load green
	add $t2, $t2, $t1
	sll $t2, $t2, 8 #shift left 8 casas
	
	lw $t1, 8($t0) #load blue
	add $t2, $t2, $t1 #cor final

	#Calculo do tamanho do bitmap
	add $t3,$t3,64
	mul $t3,$t3,$t3
	mul $t3,$t3,4
	#Calculo do endereço final do bit map
	add $t4,$t4,$t6
	add $t4,$t4,$t3 # -> endereço final

	loop:
		sw $t2,0($t6) #armazena a cor no pixel designado por 0($t6)
		add $t6,$t6,4 #desloca o ponteiro que aponta para os pixeis individuais do bitmap
		beq $t6,$t4,drawaxis # verifica se todos os pixeis estão preenchidos
		j loop	
#=========================== Fim ===========================

#=========================== DRAWAXIS ===========================
# 3.4.5 - Função que desenha uma linha preta no meio do ecrã
# s0 = endereço de rgb
# s1 = linha do meio
# s2 = coluna
# s3 = cor preta r=g=b=0
drawaxis:
	la $s0,rgb			# s0 = endereço da tabela rgb
	
	addi $s1,$zero,64		# s1 = Numero total de linhas = 64
	div $s1,$s1,2			# s1 = metade das linhas
	addi $s1,$s1,-1			# Como as linhas vai de 0 a 63 então, (total/2)-1 = centro
	
	addi $s2,$zero,0		# s2 = colunas
	
	addi $s3,$zero,0		# Como a cor preta é composta por r=0,g=0,b=0, então s3 = 0
	
	sw $s3,0($s0)			# R = 0
	sw $s3,4($s0)			# G = 0
	sw $s3,8($s0)			# B = 0
	
	addi $a2,$s0,0			# a2 = endereço rgb
	addi $a0,$s1,0			# a0 = linha
	
	loop_draw:
		addi $a1,$s2,0		# a1 = coluna
		jal setpixel		# vai a setpixel e introduz o pixel na posição desejada
		addi $s2,$s2,1		# é incrementada para a proxima coluna
		ble $s2,63,loop_draw	# Enquanto for menor que as colunas todas, repete
	
	j exit
#=========================== Fim ===========================

#=========================== PLOT ===========================
# 3.4.6 - Função que desenha o gráfico
# s0 = endereço do rgb
# s1 = endereço da tabela de valores
# s2 = linha do meio
# s3 = inteiro da tebela
# s4 = coluna
# s5 = inteiro para enviar para setpixel
plot:
	addi $s0,$a2,0			# s0 = a1 = rgb
	addi $s1,$a0,0			# s1 = a0 = tabela com os valores
	
	addi $s2,$zero,64		# s2 = Numero total de linhas = 64
	div $s2,$s2,2			# s2 = metade das linhas
	addi $s2,$s2,-1			# Como as linhas vai de 0 a 63 então, (total/2)-1 = centro
	
	addi $s4,$zero,0		# s4 = colunas, que começa em zero até 63
	
	loop_plot:
		lw $s3,0($s1)		# s3 = tab_valore[i] = ponto do grafico
		
		# Como os valores negativos estão depois da linha do meio, e os valores vêm com o sinal negativo
		# e tenho de adicionar a linha do meio mais a linha que está o ponto então multiplico por -1 os valores da tabela.
		# Os valores positivos estão antes da linha do meio, então tenho que subtrair a linha do meio para o ponto
		# que deve ser desenhado no grafico. Por isso, multiplico por -1. 
		mul $s3,$s3,-1		
		
		add $s5,$s2,$s3		# s5 = linha do meio + linha que deve desenhar o ponto
		addi $a0,$s5,0		# a0 = linha
		addi $a1,$s4,0		# a1 = coluna
		addi $s1,$s1,4		# Proximo inteiro da tabela
		jal setpixel		# vai a setpixel e introduz o pixel na posição desejada
		addi $s4,$s4,1		# é incrementada para a proxima coluna
		ble $s4,63,loop_plot	# Enquanto for menor que as colunas todas, repete
		
	j exit		
#=========================== Fim ===========================

#------------------------------------------------------------
#------------------------------------------------------------
# t1 = \0
# t2 = \n
# t3 = endereço para string(nome do ficheiro)
# t4 = nome_ficheiro[i]
# s0 = endereço dos dados que contem os valores
# s1 = endereço do cval
# s2 = caracter = buffer[i]
# s3 = contabiliza quantos numeros já estão na tabela
# s4 = contabiliza quantos caracteres estão no cval
# s5 = endereço da tabela de valores
le_ficheiro:	
	addi $t1,$zero,0		# $t1=\0=NULL
	addi $t2,$zero,10		# $t2=\n=LF Nova Linha

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
	
	#opens file
	li   $v0, 13       # system call for open file
	la   $a0, filename  # board file name
	li   $a1, 0        # Open for reading
	li   $a2, 0
	syscall

	# Verifica se nao deu erro
	bltz $v0, error1
	move $a0,$v0	# se nao der erro, entao $a0=file descriptor do ficheiro

	#read from file
	li   $v0, 14       # system call for read from file
	la   $a1, buffer   # address of buffer to which to read
	li   $a2, 2700      # hardcoded buffer length
	syscall    
	
	# Verifica se nao deu erro
	bltz $v0, error2
	move $a1, $a0	# se nao der erro, entao $a0=file descriptor do ficheiro        # read from file
				
	# Close the file 
	li   $v0, 16       # system call for close file
	syscall            # close file
	
	la $s0,buffer # s0 = endereço dos dados lidos do ficheiro
	la $s1,cval # s1 = endereço da variavel cval, onde é arvazenado os 3 caracteres para ser convertido para int
	la $s5,tabela_valores # s5 = endereço da tabela onde é armazenado os inteiros já convertidos
	addi $s3,$zero,0
	
	# Loop que vai procurar os sinais + ou -, visto que o primeiro caracter dos numeros é +/-
	loop_cval:
		addi $s4,$zero,0	# s4 = 0, visto que o s4 contabiliza os 3 caracteres para introduzir no cval
		lb $s2,0($s0)		# s2 = buffer[i] = caracter do ficheiro lido
		beq $s2,45,cria_cval	# Se o primeiro caracter for (-), vai para cria_cval
		beq $s2,43,cria_cval	# Se o primeiro caracter for (+), vai para cria_cval
		
		addi $s0,$s0,1		# É incrementado para o proximo caracter ou seja i++
		blt $s3,64,loop_cval	# s3 = numeros que já foram convertidos, logo, s3 < 64, então ainda faltam numeros para converter	
		
		jal pede_rgb		# Assim que todos os numeros forem convertidos, pede os dados para as cores
		
		la $a0,tabela_valores	# a0 = endereço da tabela de valores convertidos, ou seja, inteiros, a0 argumento
		la $a2,rgb		# a2 = endereço da tabela com a informaçao para a cor, para ser passado como argumento a2
		j plot			# Vai para plot para imprimir os dados
	
	# Ciclo que introduz os caracter no cval para ser convertido no inteiro	
	cria_cval:
		sb $s2,0($s1)		# cval[i] = s2 = buffer[i] = caracter do ficheiro lido
		addi $s0,$s0,1		# É incrementado para o proximo caracter ou seja i++
		addi $s1,$s1,1		# É incrementado para o proximo caracter ou seja i++, ou seja, cval[i++]
		lb $s2,0($s0)		# s2 = buffer[i] = caracter do ficheiro lido
		addi $s4,$s4,1		# s4 += 1, visto que o s4 contabiliza os 3 caracteres para introduzir no cval
		blt $s4,3,cria_cval	# Se s4 < 3, quer dizer que ainda faltam caracteres para ser introduzidos em cval
	
	la $a0,cval			# a0 = endereço do cval em ascii para ser convertido em int
	jal asciitoint			# vai para funcao asciitoint 
	
	sw $v0,0($s5)			# Armazena o inteiro(convertido) retornado na tabela de inteiros
	addi $s5,$s5,4			# incrementa o indice da tabela de inteiros
	addi $s3,$s3,1			# s3 = numeros que já foram convertidos, logo, s3++, o que quer dizer que foi convertido mais 1 int
	la $s1,cval			# s1 = endereço do cval, ou seja, tem que se inicializar o s1
	j loop_cval			# vai para loop_cval para procurar o proximo numero em ascii
#------------------------------------------------------------
#------------------------------------------------------------

pede_rgb:
	la $t1,rgb

	# Pede Vermelho
	li $v0, 4
	la $a0, request_red
	syscall
	
	# Pede o numero
	li $v0,5
	syscall

	sw $v0, 0($t1) #store da cor vermelha
	addi $t1,$t1,4 #incrementa $t1
	# Pede Verde
	li $v0, 4
	la $a0, request_green
	syscall
	
	# Pede o numero
	li $v0,5
	syscall
	
	sw $v0, 0($t1)  #store da cor verde
	addi $t1,$t1,4 #incrementa $t1

	# Pede Azul
	li $v0, 4
	la $a0, request_blue
	syscall
	
	# Pede o numero
	li $v0,5
	syscall
	
	sw $v0, 0($t1)  #store da cor azul
	addi $t1,$t1,4 #incrementa $t1
	
	la $a2,rgb # endereço de rgb é passado por a2
	
	jr $ra
	
#------------------------------------------------------------
#------------------------------------------------------------

error1:

	# Imprime pedido de indicação sobre execução do programa
	li $v0, 4
	la $a0, error_open
	syscall
	
	j exit
	
#------------------------------------------------------------
#------------------------------------------------------------

error2:

	# Imprime pedido de indicação sobre execução do programa
	li $v0, 4
	la $a0, error_read
	syscall

#=========================== Termina o Programa ===========================
exit:
	li $v0, 10
	syscall
