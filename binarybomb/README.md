# Binary Bomb

### Phase 1

```
0000000000400f50 <phase_1>:
  400f50:	48 83 ec 08          	sub    $0x8,%rsp
  400f54:	be 10 27 40 00       	mov    $0x402710,%esi
```
> x/s 0x402710

就直接出来了。(根据版本不同结果不同)
```
I can see Russia from my house!
```


### Phase 2

这道题要求输入某种格式的 6 个数字到 read_six_numbers。

可以从函数内部sscanf之前的地址 `0x402b1f`中找到输出格式
```asm
  401872:	be 1f 2b 40 00       	mov    $0x402b1f,%esi
  401877:	b8 00 00 00 00       	mov    $0x0,%eax
  40187c:	e8 ff f3 ff ff       	callq  400c80 <__isoc99_sscanf@plt>
```
```
%d %d %d %d %d %d
```

几种题型, 其中一种是从0开始加n，n++。

```asm
0000000000400f6c <phase_2>:
  400f6c:	55                   	push   %rbp
  400f6d:	53                   	push   %rbx
  400f6e:	48 83 ec 28          	sub    $0x28,%rsp
  400f72:	48 89 e6             	mov    %rsp,%rsi
  400f75:	e8 64 08 00 00       	callq  4017de <read_six_numbers>
  400f7a:	83 3c 24 00          	cmpl   $0x0,(%rsp)   # [0] >= 0
  400f7e:	79 05                	jns    400f85 <phase_2+0x19>
  400f80:	e8 f8 06 00 00       	callq  40167d <explode_bomb>
  400f85:	48 8d 5c 24 04       	lea    0x4(%rsp),%rbx  # rbx = [n+1]
  400f8a:	bd 01 00 00 00       	mov    $0x1,%ebp       # ebp = 1
  400f8f:	89 e8                	mov    %ebp,%eax    -> # eax = ebp = n
  400f91:	03 43 fc             	add    -0x4(%rbx),%eax # eax = [n]+n
  400f94:	39 03                	cmp    %eax,(%rbx)     # eax ?= [n+1] = [1]
  400f96:	74 05                	je     400f9d <phase_2+0x31>  ==
  400f98:	e8 e0 06 00 00       	callq  40167d <explode_bomb>
  400f9d:	83 c5 01             	add    $0x1,%ebp    # ebp+1 == 2
  400fa0:	48 83 c3 04          	add    $0x4,%rbx    # rbx+4 = 4
  400fa4:	83 fd 06             	cmp    $0x6,%ebp    # ebp ?== 6
  400fa7:	75 e6                	jne    400f8f <phase_2+0x23>
  400fa9:	48 83 c4 28          	add    $0x28,%rsp
  400fad:	5b                   	pop    %rbx
  400fae:	5d                   	pop    %rbp
  400faf:	c3                   	retq     
```

> f(0) = 0
> f(n) = f(n-1) + n

```
0 1 3 6 10 15
```

另一种题型是斐波那契数列

```asm
0000000000400f6c <phase_2>:
  400f6c:	55                   	push   %rbp
  400f6d:	53                   	push   %rbx
  400f6e:	48 83 ec 28          	sub    $0x28,%rsp
  400f72:	48 89 e6             	mov    %rsp,%rsi
  400f75:	e8 14 08 00 00       	callq  40178e <read_six_numbers>
  400f7a:	83 3c 24 00          	cmpl   $0x0,(%rsp)       # [1] == 0
  400f7e:	75 07                	jne    400f87 <phase_2+0x1b>
  400f80:	83 7c 24 04 01       	cmpl   $0x1,0x4(%rsp)    # [2] == 1
  400f85:	74 05                	je     400f8c <phase_2+0x20>
  400f87:	e8 a1 06 00 00       	callq  40162d <explode_bomb>
  400f8c:	48 8d 5c 24 08       	lea    0x8(%rsp),%rbx    # rbx = [3]
  400f91:	48 8d 6c 24 18       	lea    0x18(%rsp),%rbp   # rbp = [7]
  400f96:	8b 43 f8             	mov    -0x8(%rbx),%eax   # eax = [1]
  400f99:	03 43 fc             	add    -0x4(%rbx),%eax   # eax = [n]+[n+1]
  400f9c:	39 03                	cmp    %eax,(%rbx)       # eax ?= [n+2]
  400f9e:	74 05                	je     400fa5 <phase_2+0x39>
  400fa0:	e8 88 06 00 00       	callq  40162d <explode_bomb>
  400fa5:	48 83 c3 04          	add    $0x4,%rbx
  400fa9:	48 39 eb             	cmp    %rbp,%rbx
  400fac:	75 e8                	jne    400f96 <phase_2+0x2a>
  400fae:	48 83 c4 28          	add    $0x28,%rsp
  400fb2:	5b                   	pop    %rbx
  400fb3:	5d                   	pop    %rbp
  400fb4:	c3                   	retq   
```

> f(n) = f(n-1) + f(n-2)
> f(0) = 0, f(1) = 1, f(2) = 1, f(3) = 2, f(4) = 3, f(5) = 5, f(6) = 8, f(7) = 13, f(8) = 21, f(9) = 55

```
0 1 1 2 3 5
```
