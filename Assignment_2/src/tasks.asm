	# ficheiro com codigo a testar
	# deve escrever o codigo num ficheiro separado
	# e usar a opção do MARS "assemble all files in directory".
	# Note que o seu código deve ter todas funções aqui chamadas.
	.data 
STRING_main: .asciiz "Tarefa Principal\n"
STRING_T0: .asciiz "Tarefa 0\n"
STRING_T1: .asciiz "Tarefa 1\n"
STRING_T2: .asciiz "Tarefa 2\n"

	.text
main:
#codigo de iniciacao	
	jal Init
	
# AddThread (t0)
	la $a0,t0
	jal AddThread
	
# AddThread(t1)	
	la $a0,t1
	jal AddThread
	
# AddThread(t2)
	la $a0,t2
	jal AddThread

# startMultiThreading()
	jal startMultiThreading
	
infinito: 
	# faz qualquer coisa
	la $a0, STRING_main
	li $v0, 4
	syscall
	b infinito
	
# as tarefas
t0: 	la $a0, STRING_T0
	li $v0, 4
	syscall
	b t0

t1: 	la $a0, STRING_T1
	li $v0, 4
	syscall
	b t1

t2: 	la $a0, STRING_T2
	li $v0, 4
	syscall
	b t2

# FIM
