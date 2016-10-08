# Binary Bomb

### Phase 1

```
0000000000400f50 <phase_1>:
  400f50:	48 83 ec 08          	sub    $0x8,%rsp
  400f54:	be 10 27 40 00       	mov    $0x402710,%esi
```
> (gdb): x/s 0x402710

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

> %d %d %d %d %d %d

这题没找到什么简单的方法，只能一行一行看。


几种题型, 其中一种是从0开始加n，n++。

```asm
0000000000400f6c <phase_2>:
  400f6c:	55                   	push   %rbp
  400f6d:	53                   	push   %rbx
  400f6e:	48 83 ec 28          	sub    $0x28,%rsp
  400f72:	48 89 e6             	mov    %rsp,%rsi
  400f75:	e8 64 08 00 00       	callq  4017de <read_six_numbers>
  400f7a:	83 3c 24 00          	cmpl   $0x0,(%rsp)              # [0] >= 0
  400f7e:	79 05                	jns    400f85 <phase_2+0x19>
  400f80:	e8 f8 06 00 00       	callq  40167d <explode_bomb>
  400f85:	48 8d 5c 24 04       	lea    0x4(%rsp),%rbx           # rbx = [n+1]
  400f8a:	bd 01 00 00 00       	mov    $0x1,%ebp                # ebp = 1
  400f8f:	89 e8                	mov    %ebp,%eax             -> # eax = ebp = n
  400f91:	03 43 fc             	add    -0x4(%rbx),%eax          # eax = [n]+n
  400f94:	39 03                	cmp    %eax,(%rbx)              # eax ?= [n+1] = [1]
  400f96:	74 05                	je     400f9d <phase_2+0x31>    # ==
  400f98:	e8 e0 06 00 00       	callq  40167d <explode_bomb>
  400f9d:	83 c5 01             	add    $0x1,%ebp                # ebp+1 == 2
  400fa0:	48 83 c3 04          	add    $0x4,%rbx                # rbx+4 = 4
  400fa4:	83 fd 06             	cmp    $0x6,%ebp                # ebp ?== 6
  400fa7:	75 e6                	jne    400f8f <phase_2+0x23>
  400fa9:	48 83 c4 28          	add    $0x28,%rsp
  400fad:	5b                   	pop    %rbx
  400fae:	5d                   	pop    %rbp
  400faf:	c3                   	retq     
```

> f(0) = 0
> _f(n) = f(n-1) + n_

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
  400f7a:	83 3c 24 00          	cmpl   $0x0,(%rsp)              # [1] == 0
  400f7e:	75 07                	jne    400f87 <phase_2+0x1b>
  400f80:	83 7c 24 04 01       	cmpl   $0x1,0x4(%rsp)           # [2] == 1
  400f85:	74 05                	je     400f8c <phase_2+0x20>
  400f87:	e8 a1 06 00 00       	callq  40162d <explode_bomb>
  400f8c:	48 8d 5c 24 08       	lea    0x8(%rsp),%rbx           # rbx = [3]
  400f91:	48 8d 6c 24 18       	lea    0x18(%rsp),%rbp          # rbp = [7]
  400f96:	8b 43 f8             	mov    -0x8(%rbx),%eax          # eax = [1]
  400f99:	03 43 fc             	add    -0x4(%rbx),%eax          # eax = [n]+[n+1]
  400f9c:	39 03                	cmp    %eax,(%rbx)              # eax ?= [n+2]
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

> _f(n) = f(n-1) + f(n-2)_
> f(0) = 0, f(1) = 1, f(2) = 1, f(3) = 2, f(4) = 3, f(5) = 5, f(6) = 8, f(7) = 13, f(8) = 21, f(9) = 55

```
0 1 1 2 3 5
```


### Phase 3

第三题也有几种情况。

这次要输入的格式是两个整型。

> %d %d

首先这里， 输入的第一个数不能大于7.

```asm
400fd7:	83 7c 24 08 07       	cmpl   $0x7,0x8(%rsp)
400fdc:	77 66                	ja     401044 <phase_3+0x94>
```

接下来看着像一个 switch：

这里，将第一个参数给%eax，然后有个跳转，

```asm
400fe7:	8b 44 24 04          	mov    0x4(%rsp),%eax
400feb:	ff 24 c5 40 28 40 00 	jmpq   *0x402840(,%rax,8)
```

这里明显有一个跳转地址表。

> x/16wx 0x402840

结果很明显是下面case的地址，所以不用管直接往下看。


其中一个情况是，到最后会用第一个参数选的那个值与我们的第二个参数作比较。一样就通过

```asm
400fee:	b8 4e 00 00 00       	mov    $0x4e,%eax
400ff3:	eb 3b                	jmp    401030 <phase_3+0x7b>
400ff5:	b8 9a 03 00 00       	mov    $0x39a,%eax
400ffa:	eb 34                	jmp    401030 <phase_3+0x7b>

    ...

401030:	3b 44 24 0c          	cmp    0xc(%rsp),%eax
401034:	74 05                	je     40103b <phase_3+0x86>
401036:	e8 f2 05 00 00       	callq  40162d <explode_bomb>
40103b:	48 83 c4 18          	add    $0x18,%rsp
40103f:	c3                   	retq   
```

因为 `0x4e` 的值等于 78 所以最后的结果是：

```
0 78
```

**********************************************

还有一种情况是switch没有break，很讨厌了，

```asm
400fe9:	b8 00 00 00 00       	mov    $0x0,%eax
400fee:	eb 05                	jmp    400ff5 <phase_3+0x45>
400ff0:	b8 50 00 00 00       	mov    $0x50,%eax
400ff5:	2d f7 00 00 00       	sub    $0xf7,%eax
400ffa:	eb 05                	jmp    401001 <phase_3+0x51>
400ffc:	b8 00 00 00 00       	mov    $0x0,%eax
401001:	05 bb 00 00 00       	add    $0xbb,%eax
401006:	eb 05                	jmp    40100d <phase_3+0x5d>
401008:	b8 00 00 00 00       	mov    $0x0,%eax
40100d:	2d 1a 03 00 00       	sub    $0x31a,%eax
401012:	eb 05                	jmp    401019 <phase_3+0x69>
401014:	b8 00 00 00 00       	mov    $0x0,%eax
401019:	05 1a 03 00 00       	add    $0x31a,%eax
40101e:	eb 05                	jmp    401025 <phase_3+0x75>
401020:	b8 00 00 00 00       	mov    $0x0,%eax
401025:	2d 1a 03 00 00       	sub    $0x31a,%eax
40102a:	eb 05                	jmp    401031 <phase_3+0x81>
40102c:	b8 00 00 00 00       	mov    $0x0,%eax
401031:	05 1a 03 00 00       	add    $0x31a,%eax
401036:	eb 05                	jmp    40103d <phase_3+0x8d>
401038:	b8 00 00 00 00       	mov    $0x0,%eax
40103d:	2d 1a 03 00 00       	sub    $0x31a,%eax
401042:	eb 0a                	jmp    40104e <phase_3+0x9e>
```

而且除了不能大于7还有个一判断在最后

```asm
40104e:	83 7c 24 08 05       	cmpl   $0x5,0x8(%rsp)
401053:	7f 06                	jg     40105b <phase_3+0xab>
```

大于5就爆炸，所以选最后一项省事是不可以的。幸好最后几项都是加减同一数，最后结果也是 0，所以最后结果是：

```
4 0
```

**********************************************

另一种结果，这一结果跟网上的一样，他的输入格式变了。

> %d %c %d

要求输入一个数字，一个字符，一个数字。

```asm
4010e8:	b8 70 00 00 00       	mov    $0x70,%eax
4010ed:	3a 44 24 0f          	cmp    0xf(%rsp),%al
4010f1:	74 05                	je     4010f8 <phase_3+0x148>
```

这里从 ASCII 表里找 _0x70_ 对应 _112_ 是 `p`

其他的跟上以情况一样,所以结果是：

```
0 p 98
```


# Phase 4:
