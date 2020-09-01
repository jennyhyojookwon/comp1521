# board1.s ... Game of Life on a 10x10 grid

	.data

N:	.word 10  # gives board dimensions

board:
	.byte 1, 0, 0, 0, 0, 0, 0, 0, 0, 0
	.byte 1, 1, 0, 0, 0, 0, 0, 0, 0, 0
	.byte 0, 0, 0, 1, 0, 0, 0, 0, 0, 0
	.byte 0, 0, 1, 0, 1, 0, 0, 0, 0, 0
	.byte 0, 0, 0, 0, 1, 0, 0, 0, 0, 0
	.byte 0, 0, 0, 0, 1, 1, 1, 0, 0, 0
	.byte 0, 0, 0, 1, 0, 0, 1, 0, 0, 0
	.byte 0, 0, 1, 0, 0, 0, 0, 0, 0, 0
	.byte 0, 0, 1, 0, 0, 0, 0, 0, 0, 0
	.byte 0, 0, 1, 0, 0, 0, 0, 0, 0, 0

newBoard: .space 100

# COMP1521 19t2 ... Game of Life on a NxN grid
#
# Written by <<HyoJoo Kwon (Jenny)>>, June 2019

## Requires (from `boardX.s'):
# - N (word): board dimensions
# - board (byte[][]): initial board state
# - newBoard (byte[][]): next board state


## Global data
	.data
msg1:	.asciiz "# Iterations: "
msg2:	.asciiz "=== After iteration \n"
msg3:	.asciiz " ==="
msg4:	.asciiz "."
msg5:	.asciiz "#"
msg6:	.asciiz "\n"
ret: 	.space 1


## Provides:
	.globl	main
	.globl	decideCell
	.globl	neighbours
	.globl	copyBackAndShow


########################################################################
# .TEXT <main>
	.text
main:

# Frame:	...
# Uses:		...
# Clobbers:	...

# Locals:	...

# Structure:
#	main
#	-> [prologue]
#	-> ...
#	-> [epilogue]

# Code:

	# Your main program code goes here.  Good luck!

	# prologue 
	# set up stack frame
	addi	$sp, $sp, -4
	sw	$fp, ($sp)	# push $fp
	la	$fp, ($sp)
	addi	$sp, $sp, -4
	sw	$ra, ($sp)	# push $ra
	addi	$sp, $sp, -4
	sw	$s0, ($sp)	# push $s0
	addi	$sp, $sp, -4
	sw	$s1, ($sp)	# push $s1
	addi	$sp, $sp, -4
	sw	$s2, ($sp)	# push $s2
	addi	$sp, $sp, -4
	sw	$s3, ($sp)	# push $s3
	addi	$sp, $sp, -4
	sw	$s4, ($sp)	# push $s4
	addi	$sp, $sp, -4
	sw	$s5, ($sp)	# push $s5
	addi	$sp, $sp, -4
	sw	$s6, ($sp)	# push $s6
	addi	$sp, $sp, -4
	sw	$s7, ($sp)	# push $s7

	la	$a0, msg1
	li	$v0, 4
	syscall			    # printf("# Iternations: ")

	li	$v0, 5
	syscall		    	# scanf("%d", into $v0)

	move $s0, $v0   	# s0 = maxiters
	lw	 $s1, N	        # N

n_initialize:
	li		$s2, 1	    # n

n_condition:
	bgt		$s2, $s0, end_main	# if n > maxiters, end_main
	#nop

i_initialize:
	li		$s3, 0	# i
	
i_condition:
	bge		$s3, $s1, i_done	# if i >= N, end condition
	#nop

j_initialize:
	li		$s4, 0	# j
	
j_condition:
	bge		$s4, $s1, j_done	# if j >= N, end condition
	#nop

	move	$a0, $s3		
	move	$a1, $s4		
	jal	neighbours		    # neighbours(i,j)
	#nop	

	move 	$t3, $v0    	# nn = neighbours(i,j)


	# newboard[i][j]
	mul		$t1, $s1, $s2			# t1 = N * current row
	add		$t1, $t1, $s3			# + current col
	lw		$v0, newBoard($t1)		# v0 = newboard[i][j]
	la		$v1, newBoard($t1)		# v1 = &newboard[i][j]
	move	$s6, $v1				# s6 = &newboard[i][j]

	# board[i][j]
	mul		$t1, $s1, $s2			# t1 = N * current row
	add		$t1, $t1, $s3			# + current col
	lw		$v0, board($t1)			# v0 = board[i][j]
	la		$v1, board($t1)			# v1 = &board[i][j]
	move	$s7, $v1				# s7 = &board[i][j]

	# decideCell (board[i][j], nn)
	move	$a0, $s7	    	# &board[i][j]
	move	$a1, $t3	    	# nn
	jal	    decideCell	    	# decideCell (board[i][j], nn)

	move	$s6, $v0	        # newboard[i][j] = decideCell (board[i][j], nn)
	
j_increment:
	addi	$s4, $s4, 1		    # increase j counter
	j		j_condition	    	# jump to j_condition
	#nop

j_done:

i_increment:
	addi	$s3, $s3, 1	    	# increase i counter
	j		i_condition	    	# jump to i_condition
	#nop

i_done:
    la	$a0, msg2
	li	$v0, 4
	syscall			            # printf("=== After iteration \n")

	move	$a0, $s2        	# assume $s2 holds n
	li	$v0, 1
	syscall		             	# printf ("%d", tmp)


	la	$a0, msg3
	li	$v0, 4
	syscall		            	# printf(" ===\n")

	jal	copyBackAndShow	        # copyBackAndShow();   

n_increment:
	addi	$s2, $s2, 1	    	# increase n counter
	j		n_condition	    	# jump to j_condition
	#nop

n_done:
	

end_main:
	li	$v0, 0  	# return 0
	

	# epilogue
	# tear down stack frame
	lw	$s7, ($sp)	# push $s7
	addi	$sp, $sp, 4
	lw	$s6, ($sp)	# push $s6
	addi	$sp, $sp, 4
	lw	$s5, ($sp)	# push $s5
	addi	$sp, $sp, 4
	lw	$s4, ($sp)	# pop $s4
	addi	$sp, $sp, 4
	lw	$s3, ($sp)	# pop $s3
	addi	$sp, $sp, 4
	lw	$s2, ($sp)	# pop $s2
	addi	$sp, $sp, 4
	lw	$s1, ($sp)	# pop $s1
	addi	$sp, $sp, 4
	lw	$s0, ($sp)	# pop $s0
	addi	$sp, $sp, 4
	lw	$ra, ($sp)	# pop $ra
	addi	$sp, $sp, 4
	lw	$fp, ($sp)	# pop $fp
	addi	$sp, $sp, 4

main__post:
	jr	$ra

	# Put your other functions here
	
########################################################################
# .TEXT <decideCell>
	.text
decideCell:

	# prologue 
	# set up stack frame
	addi	$sp, $sp, -4
	sw	$fp, ($sp)	# push $fp
	la	$fp, ($sp)
	addi	$sp, $sp, -4
	sw	$ra, ($sp)	# push $ra
	addi	$sp, $sp, -4
	sw	$s0, ($sp)	# push $s0
	addi	$sp, $sp, -4
	sw	$s1, ($sp)	# push $s1
	addi	$sp, $sp, -4
	sw	$s2, ($sp)	# push $s2
	addi	$sp, $sp, -4
	sw	$s3, ($sp)	# push $s3
	addi	$sp, $sp, -4
	sw	$s4, ($sp)	# push $s4

	move $s1, $a0	    # s1 = old
	move $s2, $a1	    # s2 = nn

	li	$t1, 1
	li	$t2, 3
	li	$t3, 2

	beq		$s1, $t1, if_old 		# if $s1 == 1 then if_old
	beq		$s2, $t2, if_nn			# if $s2 == 3 then if_nn
	j		ret_zero				# jump to ret_zero
	
if_old:
	blt		$s2, $t3, ret_zero		# if $s2 < 2 then ret_zero
	beq		$s2, $t3, ret_one		# if $s2 == 2 then ret_one
	beq		$s2, $t2, ret_one		# if $s2 == 3 then ret_one
	j		ret_zero				# jump to ret_zero
	
if_nn:
	j		ret_one					# jump to ret_one
	
ret_zero:
	lw	    $t6, ret		
	li	    $t7, 0		
	sw	    $t7, ret		
	move    $v0, $t7
	j		end_decideCell			# jump to end_function

ret_one:
	lw	    $t6, ret		
	li  	$t7, 1		
	sw	    $t7, ret		
	move    $v0, $t7

end_decideCell:


	# epilogue
	# tear down stack frame
	lw	$s4, ($sp)	# pop $s4
	addi	$sp, $sp, 4
	lw	$s3, ($sp)	# pop $s3
	addi	$sp, $sp, 4
	lw	$s2, ($sp)	# pop $s2
	addi	$sp, $sp, 4
	lw	$s1, ($sp)	# pop $s1
	addi	$sp, $sp, 4
	lw	$s0, ($sp)	# pop $s0
	addi	$sp, $sp, 4
	lw	$ra, ($sp)	# pop $ra
	addi	$sp, $sp, 4
	lw	$fp, ($sp)	# pop $fp
	addi	$sp, $sp, 4

	jr	$ra


########################################################################
# .TEXT <neighbours>
	.text
neighbours:

	# prologue 
	# set up stack frame
	addi	$sp, $sp, -4
	sw	$fp, ($sp)	# push $fp
	la	$fp, ($sp)
	addi	$sp, $sp, -4
	sw	$ra, ($sp)	# push $ra
	addi	$sp, $sp, -4
	sw	$s0, ($sp)	# push $s0
	addi	$sp, $sp, -4
	sw	$s1, ($sp)	# push $s1
	addi	$sp, $sp, -4
	sw	$s2, ($sp)	# push $s2
	addi	$sp, $sp, -4
	sw	$s3, ($sp)	# push $s3
	addi	$sp, $sp, -4
	sw	$s4, ($sp)	# push $s4
	addi	$sp, $sp, -4
	sw	$s5, ($sp)	# push $s5
	addi	$sp, $sp, -4
	sw	$s6, ($sp)	# push $s6
	addi	$sp, $sp, -4
	sw	$s7, ($sp)	# push $s7


	li	$s0, 0		    # s0 = nn = 0
	move $s1, $a0	    # s1 = i
	move $s2, $a1	    # s2 = j

	li	$t0, 1

x_initialize:
	li		$s3, -1	# x
	
x_condition:
	bgt		$s3, $t0, x_done	# if i x > 1, end condition
	#nop

y_initialize:
	li		$s4, -1	    # y
	
y_condition:
	bgt		$s4, $t0, y_done	# if y > 1, end condition
	#nop

	lw	 $s5, N
	li	 $s6, 0

	add	    $t1, $s1, $s3	    # i + x
	addi	$t2, $s5, -1	    # N - 1
	add	    $t3, $s2, $s4	    # j + y

first_if:
	blt		$t1, $s6, second_if
	bgt		$t1, $t2, second_if
    j		y_increment				# jump to y_increment
    

second_if:
	blt		$t3, $s6, third_if
	bgt		$t3, $t2, third_if
     j		y_increment				# jump to y_increment

third_if:
	bnez	$s3, y_increment
	bnez	$s4, y_increment

fourth_if:
	# board[i+x][j+y]
	mul		$t1, $s5, $t1			# t1 = N * (i+x)
	add		$t1, $t1, $t3			# + (j+y)
	lw		$v0, board($t1)			# v0 = board[i+x][j+y]
	la		$v1, board($t1)			# v1 = &board[i+x][j+y]
	move	$s7, $v1				# s7 = &board[i][j]
	

	li		$t6, 1
	beq		$s7, $t6, nn_increase	# if (board[i + x][j + y] == 1) nn++
	j		y_increment				# jump to y_increment
	

nn_increase:
	add	$s0, $s0, 1
	
y_increment:
	addi	$s4, $s4, 1		# increase y counter
	j		y_condition		# jump to j_condition
	#nop

y_done:

x_increment:
	addi	$s3, $s3, 1		# increase x counter
	j		x_condition		# jump to x_condition
	#nop

x_done:

end_neighbors:
	move	$v0, $s0 	    # return nn



	# epilogue
	# tear down stack frame
	lw	$s7, ($sp)	# pop $s7
	addi	$sp, $sp, 4
	lw	$s6, ($sp)	# pop $s6
	addi	$sp, $sp, 4
	lw	$s5, ($sp)	# pop $s5
	addi	$sp, $sp, 4
	lw	$s4, ($sp)	# pop $s4
	addi	$sp, $sp, 4
	lw	$s3, ($sp)	# pop $s3
	addi	$sp, $sp, 4
	lw	$s2, ($sp)	# pop $s2
	addi	$sp, $sp, 4
	lw	$s1, ($sp)	# pop $s1
	addi	$sp, $sp, 4
	lw	$s0, ($sp)	# pop $s0
	addi	$sp, $sp, 4
	lw	$ra, ($sp)	# pop $ra
	addi	$sp, $sp, 4
	lw	$fp, ($sp)	# pop $fp
	addi	$sp, $sp, 4

	jr	$ra

########################################################################
# .TEXT <copyBackAndShow>
	.text
copyBackAndShow:

	# prologue 
	# set up stack frame
	addi	$sp, $sp, -4
	sw	$fp, ($sp)	# push $fp
	la	$fp, ($sp)
	addi	$sp, $sp, -4
	sw	$ra, ($sp)	# push $ra
	addi	$sp, $sp, -4
	sw	$s0, ($sp)	# push $s0
	addi	$sp, $sp, -4
	sw	$s1, ($sp)	# push $s1
	addi	$sp, $sp, -4
	sw	$s2, ($sp)	# push $s2
	addi	$sp, $sp, -4
	sw	$s3, ($sp)	# push $s3
	addi	$sp, $sp, -4
	sw	$s4, ($sp)	# push $s4


	lw	 $s2, N

row_initialize:
	li		$s0, 0	    # i = row
	
row_condition:
	bge		$s0, $s2, row_done	    # if i >= N, end condition
	#nop

col_initialize:
	li		$s1, 0      # j = col
	
col_condition:
	bge		$s1, $s2, col_done      # if j >= N, end condition
	#nop

	# newboard[i][j]
	mul		$t1, $s2, $s0			# t1 = N * current row
	add		$t1, $t1, $s1			# + current col
	lw		$v0, newBoard($t1)		# v0 = newboard[i][j]
	la		$v1, newBoard($t1)		# v1 = &newboard[i][j]
	move	$s4, $v1				# s6 = &newboard[i][j]

	# board[i][j]
	mul		$t1, $s2, $s0			# t1 = N * current row
	add		$t1, $t1, $s1			# + current col
	lw		$v0, board($t1)			# v0 = board[i][j]
	la		$v1, board($t1)			# v1 = &board[i][j]
	move	$s5, $v1				# s7 = &board[i][j]

	move	$s5, $s4				# board[i][j] = newboard[i][j]


if:
	beqz	$s5, print_dot			# if (board[i][j] == 0) putchar ('.')
	bnez	$s5, print_sign			# else putchar('#')

print_dot:
	la		$a0, msg4
	li		$v0, 4
	syscall					        # putchar ('.')
	j		col_increment           # jump to j_increment

print_sign:
	la	$a0, msg5
	li	$v0, 4
	syscall				            # putchar ('#')


col_increment:
	addi	$s1, $s1, 1	        	# increase j counter
	j		col_condition	    	# jump to j_condition
	#nop

col_done:
	la	$a0, msg6
	li	$v0, 4
	syscall			               	# putchar ('\n')

row_increment:
	addi	$s0, $s0, 1		        # increase i counter
	j		row_condition	    	# jump to i_condition
	#nop 

row_done:

	# epilogue
	# tear down stack frame
	lw	$s4, ($sp)	# pop $s4
	addi	$sp, $sp, 4
	lw	$s3, ($sp)	# pop $s3
	addi	$sp, $sp, 4
	lw	$s2, ($sp)	# pop $s2
	addi	$sp, $sp, 4
	lw	$s1, ($sp)	# pop $s1
	addi	$sp, $sp, 4
	lw	$s0, ($sp)	# pop $s0
	addi	$sp, $sp, 4
	lw	$ra, ($sp)	# pop $ra
	addi	$sp, $sp, 4
	lw	$fp, ($sp)	# pop $fp
	addi	$sp, $sp, 4
	
