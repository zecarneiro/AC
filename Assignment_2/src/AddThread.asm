# Funções e variaveis usadas por outros ficheiros, por isso, tenho que usar .globl nome_funcao/nome_variavel
# Funções
.globl AddThread

# Variáveis globais
.globl cabeca
.globl end_actual
.globl end_proximo


.data
cabeca: .word 0				# cabeca = aponta para o primeiro PCB criado

# Variável end_actual, serve para armazenar/aponta o endereço da thr que está ser executada ou criado o novo PCB
# ou seja, as variáveis end_actual, end_proximo servem como ponteiros
end_actual: .word 0
end_proximo:.word 0

.text
#=========================== AddThread ===========================
# 5.4.4 - Função AddThread
AddThread:
	add $t1,$zero,$a0		# t1 = endereço da thr_i, endereço que passado através de a0
	
	# Vai alocar memoria para o PCB da tarefa que o pediu, para guardar todos os registos necessários
	addi $v0,$zero,9
	# Tamanho em bytes a ser alocado = 33*4 = 132, ou seja, 31 registos + endereço do proximo PCB + endereço
	# da thread(EPC)			
	addi $a0, $zero, 132
	syscall				# realiza a operação
	
	sw $zero,0($v0)			# No PCB da thr_i está PCB[0] = next = null
	# No PCB da thr_i está PCB[1] = endereço das thr respectivas quando são chamadas para serem executadas
	sw $t1,4($v0)
	
	la $t2, end_actual		# t2 = endereço da variável end_actual
	lw $t3,0($t2)			# t3 = int = endereço que está na variavel end_actual
	beqz $t3,cria_lista_PCB 	# Se o end_actual = 0 quer dizer que nenhum PCB foi criado
	
	sw $v0,0($t3)			# next do PCB da thr_i -1 = endereço do novo PCB da thr_i 
	sw $v0,0($t2)			# end_actual = endereço do novo PCB da thr_i
	
	jr $ra				# retorna para a funçao onde foi efectuado o jal
	
	cria_lista_PCB:
		la $t0, cabeca		# endereço da variável cabeca		
		sw $v0,0($t0)		# cabeca = endereço do primeiro PCB
		sw $v0,0($t2)		# end_actual = cabeca	
	jr $ra				# retorna para a funçao onde foi efectuado o jal
	
	
	
	
		
