# Funções e variaveis usadas por outros ficheiros, por isso, tenho que usar .globl nome_funcao/nome_variavel
# Funções
.globl Init

.data
Init_msg: .asciiz "Iniciando o Kernel Multi-Tarefa\n"	# Mensagem para ser imprimida no ecrã

.text
#=========================== Init ===========================
# 5.4.3 - Função Init
Init:
	# Imprime string
	li $v0,4		# v0 = inteiro para imprimir uma mensagem com syscall
	la $a0,Init_msg		# a0 = string que contém a mensagem que vai ser imprimida
	syscall			# realiza a operação
	
	jr $ra			# Volta para o endereço que chamou a função Init
