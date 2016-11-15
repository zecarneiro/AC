.data
bitmap: .word 0x10010000
cval: .ascii "+65"
list: .word 32,32,255,0,0


.text
main:

	la $a0, list

	jal setpixel
		
	j exit


	

#=========================== SETPIXEL ===========================	

#Setpixel - Lu�s
#
#a0 -> tabela de cinco argumentos (linha,coluna,R,G,B)
#t0 -> armazena localmente tabela de cinco argumentos (linha,coluna,R,G,B)
#t1 -> linha
#t2 -> coluna
#t3 -> 
#t4 -> Cores
#t5 -> Cor final
#t6 -> endere�o do bitmap

#Pseudo-C�digo
#
# Armazena em $t0 o endere�o para o 1� argumento
# Armazena em $t1 a linha
# Armazena em $t2 a coluna
# Armazena em $t6 o endere�o que aponta para x=0,y=0 do bitmap
# 
# Subtrac��o de 1 unidade aos elementos linha e coluna -> assume-se que o utilizador considera os n�meros indo de 1 a 32 e n�o de 0 a 31
#
##--Achar Posi��o--##
#
# multiplica��o de $t1 (linha) por 64 e depois por 4
# multiplica��o de $t2 (coluna) por 4
#	
# Adiciona-se $t1 a $t6
# Adiciona-se $t2 a $t6
#
##--Load das cores--##
#
# load da cor RED para $t4
# adiciona $t4 a $t5
# shift left de $t5 8 casas
# load da cor Green para $t4
# adiciona $t4 a $t5
# shift left de $t5 8 casas
# load da cor Blue para $t4
# adiciona $t4 a $t5 -> $t5 cor final
#
#Com posi��o e cor calculadas faz-se o store
#Store $t5 em 0$t6
#
#
#
#
				
setpixel:

	la,$t6,bitmap

	addi $t0,$a0,0
	lw $t1, 0($t0) #load linha
	lw $t2, 4($t0) #load coluna
		
	#sub $t6, $t6, 20
	
	#Assun��o, falar com prof
	#-----------
	sub $t1, $t1, 1
	sub $t2, $t2, 1
	#-----------

	#Calculo da posi��o
	#linha
	mul $t1, $t1, 64
	mul $t1, $t1, 4
	add $t6,$t6,$t1
	#coluna
	mul $t2, $t2, 4
	#posi��o final
	add $t6,$t6,$t2
	
	#Calculo RGB
	lw $t4, 8($t0) #load red	
	add $t5, $t5, $t4
	sll $t5, $t5, 8 #shift left
	lw $t4, 12($t0) #load green
	add $t5, $t5, $t4
	sll $t5, $t5, 8 #shift left
	lw $t4, 16($t0) #load blue
	add $t5, $t5, $t4 #cor final
	
	#store da cor
	sw $t5,0($t6)
	

#=========================== Fim ===========================

exit:

	li $v0, 10
	syscall
