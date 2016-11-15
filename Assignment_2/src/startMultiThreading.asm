# Funções e variaveis usadas por outros ficheiros, por isso, tenho que usar .globl nome_funcao/nome_variavel
# Funções
.globl startMultiThreading

.data
Todas_Masc_int: .word 0x0000ff00	# Endereço com todas as mascaras de interrupções activas
Masc_int_teclado: .word 0x00010000	# Endereço da mascara de interrupção do teclado
End_KDMS: .word 0xffff0000		# Endereço do simulador keyboard and Display
.text
#=========================== startMultiThreading ===========================
# 5.4.6 - Função startMultiThreading
startMultiThreading:
	# Criação de um PCB para treadMain
	addi $a0,$ra,0		# a0 = endereço de retorno para ser armazenado no PCB
	jal AddThread		# Vai a função AddThr para criar um PCB para a threadMain
	
	jal activa_servico	# Vai para função activa_servico
	
	la $s0,cabeca		# s0 = endereço da variável cabeça que contém o int(endereço) do PCB da thr0
	lw $s1,0($s0)		# s1 = endereço do PCB da thr0
	lw $s2,4($s1)		# s2 = endereço da função thr0
	
	la $s3, end_actual	# s3 = endereço da variavel end_actual
	sw $s1,0($s3)		# s1 = endereço do PCB da primeira thr a ser executada, nesse caso, thr0
	
	la $s4, end_proximo	# s4 = endereço da variável end_proximo
	lw $s5,0($s1)		# s5 = endereço do PCB da proxima thr
	sw $s5,0($s4)		# Armazeno o endereço do PCB da proxima thr em end_proximo
	
	jr $s2			# Vai para a função thr0
	
#=========================== Serviço de Rotina ===========================
# 5.4.5 - Código de Serviço de rotina
activa_servico:
	mfc0 $t0, $12		# copia o conteudo da mascara de interrupção para t0
	
	# t1 = todas as mascaras de interrupções
	# ou seja, os bits 15 a 8 ficam activados e os restantes não.
	lw $t1, Todas_Masc_int
	
	# inverte os bits que estão em t1, ou seja, 0x0000ff00 passa para 0xffff00ff
	# resumindo, os bits 15 a 8 ficam desactivados e os restantes ficam activos
	not $t1,$t1
	
	# É desactivado todas as interrupções
	# Se t0 = 0x0000ff11 e t1 = 0xffff00ff o resultado se t0 = 0x00000011
	and $t0,$t0,$t1
	
	lw $t1, Masc_int_teclado	# t1 = mascara de interrupção do teclado
	
	# Pelo que entendi desta operação t0 terá a mascara de interrupção do teclado
	# ou nenhuma mascara. Resumindo t0 = 0x00000011 e t1 = 0x00010000, o resultado da operação
	# será t0 = 00010011  
	or $t0,$t0,$t1
	
	# Com a mascara de interrupção desejada, vou copiar a mascara para registo R12, que é o registo para a mascara
	mtc0 $t0,$12
	
	# Activação da interrupção do teclado quando digitado através do KDMS
	lw $t0,End_KDMS
	li $t1, 0x00000002
	sw $t1,0($t0)
	
	jr $ra				# Retorna para startMultiThreading na posição onde foi feito o salto
	
#=========================== Código para quando a interrupção é feita ===========================
	.kdata

	# Variaveis auxiliares, necessarias para o bom funcionamento do programa
	save_at: .word 0
	save_EPC: .word 0
	save_t0: .word 0
	save_t1: .word 0


	.ktext 0x80000180		# A interrupção começa aqui, ou seja, neste endereço
	
	mfc0 $k0,$14			# k0 = EPC
	addi $k1,$at,0			# k1 = at
	
	# Guardo o program counter, porque se uma função foi interrompida a meio, então ao voltar para ser executada
	# o kernel tem que continuar na linha onde foi interrompida
	sw $k0,save_EPC
	
	# Salvo o registo at, caso a interrupção foi feita durante uma pseudo-intrução 
	sw $k1,save_at

	# Guardo as variáveis auxiliares, visto que as vou usar
	sw $t0,save_t0
	sw $t1,save_t1
	
	# Vou buscar o endereço do PCB da thr que estava em execução antes de ser
	# interrompida
	la $t0, end_actual
	lw $t1,0($t0)			# t1 = endereço do PCB da thr_i
	
	# Como armazenei os valores dos registos at, EPC, t0 e t1, nas variáveis auxiliares
	# e não no PCB da thr_i, visto que iria precisar de Registos para ir buscar o endereço
	# da thr_i, então agora vou armazenalos no PCB da thr_i
	 
	lw $t0,save_EPC		# t0 = conteudo que estava no EPC		
	sw $t0,4($t1)		# Armazeno o conteudo do EPC no PCB da thr_i
	
	lw $t0,save_at		# t0 = conteudo que estava no at
	sw $t0,8($t1)		# Armazeno o conteudo do at no PCB da thr_i
	
	lw $t0,save_t0		# t0 = conteudo que estava no t0
	sw $t0,12($t1)		# Armazeno o conteudo do t0 no PCB da thr_i
	
	lw $t0,save_t1		# t0 = conteudo que estava no t1
	sw $t0,16($t1)		# Armazeno o conteudo do t1 no PCB da thr_i
	
	# Vou agora armazenar os restantes registos, no PCB da thr_i
	sw $v0,20($t1)
	sw $v1,24($t1)
	sw $a0,28($t1)
	sw $a1,32($t1)
	sw $a2,36($t1)
	sw $a3,40($t1)
	sw $t2,44($t1)
	sw $t3,48($t1)
	sw $t4,52($t1)
	sw $t5,56($t1)
	sw $t6,60($t1)
	sw $t7,64($t1)
	sw $t8,68($t1)
	sw $t9,72($t1)
	sw $s0,76($t1)
	sw $s1,80($t1)
	sw $s2,84($t1)
	sw $s3,88($t1)	
	sw $s4,92($t1)
	sw $s5,96($t1)
	sw $s6,100($t1)
	sw $s7,104($t1)
	sw $sp,108($t1)
	sw $fp,112($t1)
	sw $ra,116($t1)
	sw $gp,120($t1)
	
	# Vou armazenar o valor do registo hi e lo
	mfhi $k0		# k0 = hi
	sw $k0,124($t1)		# Armazeno o conteudo do hi no PCB da thr_i
	
	mflo $k0		# k0 = lo
	sw $k0,128($t1)		# Armazeno o conteudo do lo no PCB da thr_i
	
	#========= Vou agora verificar o que causou a interrupção =========
	mfc0 $k0,$13 		# k0 = conteudo do registo, ou seja, tipo de excepção
	srl $t1,$k0,2		# 
	andi $t1,$t1,0x1f	# extract bits 2-6
	
	bnez $t1, executa_thr	# Se a interrupção não veio do teclado, então continuo a executar a thr_i
	
	#========= Troca de execução de threads =========
	# NOTA: Visto que todos os registos já estão armazenados, então posso
	# usar todos sem exceção nesta parte do codigo
		
	la $s0, end_actual	# s0 = endereço da variavel end_actual
	la $s1, end_proximo	# s1 = endereço da variável end_proximo
	
	lw $s2,0($s1)		# s2 = endereço do proximo PCB
	sw $s2,0($s0)		# end_actual recebe o endereço da proxima thr que vai ser executada
	
	add $t1,$zero,$s2	# t1 vai apontar para o proximo PCB
	lw $s2,0($t1)		# s2 = endereço do PCB da thr_i+1 sendo thr_i aquela que entrar em execução
	sw $s2,0($s1)		# end_proximo vai conter o endereço do PCB da thr_i + 1
	
	# Caso o next = 0, quer dizer que já não existe mais PCBs a seguir ao PCB da thr que
	# estava a executar, então, a proxima thr que deve ser executada é a thr0 
	bnez $s2,executa_thr
	
	la $s3,cabeca		# s3 = endereço da variável/ponteiro cabeca
	lw $s4,0($s3)		# s4 = endereço do PCB da thr0
	sw $s4,0($s1)		# end_proximo vai conter o endereço do PCB da thr0	
	
	#========= Executa a thread =========
	executa_thr:
		# Visto que t1 é o stack pointer do PCB da thr_i, então irei armazenar nas variáveis auxiliares
		# os conteudos necessários para execução da proxima thr
		
		# Recupero o EPC
		lw $t0,4($t1)
		sw $t0,save_EPC
		
		# Recupero o At
		lw $t0,8($t1)
		sw $t0,save_at
		
		# Salvo o t1, visto que uso t1 como ponteiro do PCB
		lw $t0,16($t1)
		sw $t0,save_t1
		
		#========= Vou recuperar todos os registos da thr_i =========
		lw $t0,12($t1)
		lw $v0,20($t1)
		lw $v1,24($t1)
		lw $a0,28($t1)
		lw $a1,32($t1)
		lw $a2,36($t1)
		lw $a3,40($t1)
		lw $t2,44($t1)
		lw $t3,48($t1)
		lw $t4,52($t1)
		lw $t5,56($t1)
		lw $t6,60($t1)
		lw $t7,64($t1)
		lw $t8,68($t1)
		lw $t9,72($t1)
		lw $s0,76($t1)
		lw $s1,80($t1)
		lw $s2,84($t1)
		lw $s3,88($t1)	
		lw $s4,92($t1)
		lw $s5,96($t1)
		lw $s6,100($t1)
		lw $s7,104($t1)
		lw $sp,108($t1)
		lw $fp,112($t1)
		lw $ra,116($t1)
		lw $gp,120($t1)
		
		# Vou repor hi e lo
		lw $k0,124($t1)		# k0 = conteudo do hi no PCB da thr_i
		mthi $k0		# hi = k0
		
		lw $k0,128($t1)		# k0 = conteudo do do lo no PCB da thr_i
		mtlo $k0		# lo = k0
		
		# Recupero t0 e t1
		lw $t1,save_t1
		
		#========= Vou recuperar os Registos que faltam e executar a thr_i =========
		lw $k0,save_EPC		# k0 = EPC
		lw $k1,save_at		# k1 = at
		
		# Devolve o EPC, ou a posiçao onde a thr_i foi interrompida
		# Antes de devolver o EPC para o registo 14 deveriamos incrementar 4
		# Mas ao fazer o incremento, verificamos que dá erro, visto que EPC passa
		# a ter o enderço da função AddThread, mas concretamente, na linha
		# add $t1,$zero,$a0, ou seja, a primeira linha da respectiva função,
		# logo, decidimos não incrementar o endereço do EPC
		#addiu $k0,$k0,4
		mtc0 $k0,$14
		
		# Devolve at da thr_i
		move $at,$k1		
		
		# Passos Finais
		mtc0 $zero,$13
		mfc0 $k0,$12
		andi $k0, 0xfffd
		ori $k0,0x0001
		mtc0 $k0,$12
		eret			# Retorna para o endereço do EPC



