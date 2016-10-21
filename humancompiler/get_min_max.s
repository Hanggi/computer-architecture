#------------------------------------------------------------
#
#  4190.308-002: Computer Architecture (Fall 2016)
#
#  Skeleton code for Lab #2: Great human compiler
#
#  Oct 21, 2016.
#  Instructor: Jae W. Lee (jaewlee@snu.ac.kr)
#  Seoul National University
#
#------------------------------------------------------------

.text
  .align 4
.globl  get_min_max
  .type get_min_max,@function

get_min_max:
  pushq %rbp
  movq  %rsp, %rbp
  pushq %rsp

  #----------------------------------------------------
  # Do NOT use %*cx and %*dx registers
  #----------------------------------------------------
  
  # Write your codes here

    # 0x24(%rbp) == n
    # %rdi = arr[0]

    movl    (%rdi), %eax
    movl    %eax, 0x20(%rbp)        # max = arr[0]

    movl    0x4(%rdi), %eax
    movl    %eax, 0x1c(%rbp)        # min = arr[0]

    movl	  $1, -4(%rbp)            # i = 1
    jmp	.L7

.L10:
    movl    -4(%rbp), %eax
    leaq    0(,%eax,4), %rbx        # i * 4
    movq    %rdi, %rax
    addq    %rbx, %rax              # &arr + i * 4
    movl    (%rax), %eax            #
    cmpl    0x20(%rbp), %eax        # max ? arr[i]
	  jle	.L8
    movl    %eax, 0x20(%rbp)         # 

.L8:
    cmpl    0x1c(%rbp), %eax        # min ? arr[i]
	  jge	.L9

    movl    %eax, 0x1c(%rbp)         # 
.L9:
	  addl	  $1, -4(%rbp)

.L7:
    movl	  -4(%rbp), %eax          # if
    cmpl	  0x24(%rbp), %eax        # i < n
    jl	.L10


    # movl $0x20, 0x20(%rbp)  # max
    # movl $0x10, 0x1c(%rbp)  # min

  #----------------------------------------------------

  movq  %rbp, %rsp
  popq  %rbp
  retq
