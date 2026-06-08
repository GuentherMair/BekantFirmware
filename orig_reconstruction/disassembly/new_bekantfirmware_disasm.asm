
dist/bekantfirmware.elf:	file format elf32-pic

Disassembly of section cinit:

000007bb <start_initialization>:
     7bb: bf 30        	movlw	0xbf
     7bc: 84 00        	movwf	0x4
     7bd: 86 30        	movlw	0x86
     7be: 85 00        	movwf	0x5
     7bf: 98 30        	movlw	0x98
     7c0: 86 00        	movwf	0x6
     7c1: 23 30        	movlw	0x23
     7c2: 87 00        	movwf	0x7
     7c3: 00 30        	movlw	0x0
     7c4: ff 00        	movwf	0x7f
     7c5: 58 30        	movlw	0x58
     7c6: 84 31        	movlp	0x4
     7c7: 7b 24        	call	0x47b
     7c8: 87 31        	movlp	0x7
     7c9: 84 31        	movlp	0x4
     7ca: 38 24        	call	0x438
     7cb: 87 31        	movlp	0x7
     7cc: fd 00        	movwf	0x7d
     7cd: 53 30        	movlw	0x53
     7ce: 84 00        	movwf	0x4
     7cf: 84 30        	movlw	0x84
     7d0: 85 00        	movwf	0x5
     7d1: 63 30        	movlw	0x63
     7d2: 86 00        	movwf	0x6
     7d3: 00 30        	movlw	0x0
     7d4: 87 00        	movwf	0x7
     7d5: 00 30        	movlw	0x0
     7d6: ff 00        	movwf	0x7f
     7d7: 08 30        	movlw	0x8
     7d8: 84 31        	movlp	0x4
     7d9: 7b 24        	call	0x47b
     7da: 87 31        	movlp	0x7
     7db: d0 30        	movlw	0xd0
     7dc: 84 00        	movwf	0x4
     7dd: 85 30        	movlw	0x85
     7de: 85 00        	movwf	0x5
     7df: a0 30        	movlw	0xa0
     7e0: 86 00        	movwf	0x6
     7e1: 00 30        	movlw	0x0
     7e2: 87 00        	movwf	0x7
     7e3: 00 30        	movlw	0x0
     7e4: ff 00        	movwf	0x7f
     7e5: 35 30        	movlw	0x35
     7e6: 84 31        	movlp	0x4
     7e7: 7b 24        	call	0x47b
     7e8: 87 31        	movlp	0x7
     7e9: 20 30        	movlw	0x20
     7ea: 84 00        	movwf	0x4
     7eb: 00 30        	movlw	0x0
     7ec: 85 00        	movwf	0x5
     7ed: 2c 30        	movlw	0x2c
     7ee: fe 00        	movwf	0x7e
     7ef: 00 30        	movlw	0x0
     7f0: ff 00        	movwf	0x7f
     7f1: 84 31        	movlp	0x4
     7f2: 6f 24        	call	0x46f
     7f3: 87 31        	movlp	0x7
     7f4: 21 00        	movlb	0x1
     7f5: d5 01        	clrf	0x55
     7f6: d6 01        	clrf	0x56
     7f7: d7 01        	clrf	0x57
     7f8: d8 01        	clrf	0x58
     7f9: d9 01        	clrf	0x59
     7fa: da 01        	clrf	0x5a
     7fb: db 01        	clrf	0x5b
     7fc: dc 01        	clrf	0x5c

000007fd <end_of_initialization>:
     7fd: 20 00        	movlb	0x0
     7fe: 85 31        	movlp	0x5
     7ff: 52 2d        	goto	0x552

Disassembly of section text1:

00000444 <_lin_init_hw>:
     444: 84 31        	movlp	0x4
     445: 3e 24        	call	0x43e
     446: 84 31        	movlp	0x4
     447: 84 31        	movlp	0x4
     448: 39 24        	call	0x439
     449: 84 31        	movlp	0x4
     44a: 08 00        	return

Disassembly of section text2:

0000043e <_lin_reset_usart>:
     43e: 23 00        	movlb	0x3
     43f: 9d 13        	bcf	0x1d, 0x7
     440: 9d 17        	bsf	0x1d, 0x7
     441: 9e 12        	bcf	0x1e, 0x5
     442: 9e 16        	bsf	0x1e, 0x5
     443: 08 00        	return

Disassembly of section text3:

000004e6 <_bui_init>:
     4e6: 00 30        	movlw	0x0
     4e7: f6 00        	movwf	0x76
     4e8: 84 31        	movlp	0x4
     4e9: 94 24        	call	0x494
     4ea: 84 31        	movlp	0x4
     4eb: 77 08        	movf	0x77, w
     4ec: 20 00        	movlb	0x0
     4ed: ac 00        	movwf	0x2c
     4ee: 76 08        	movf	0x76, w
     4ef: ab 00        	movwf	0x2b
     4f0: 02 30        	movlw	0x2
     4f1: f6 00        	movwf	0x76
     4f2: 84 31        	movlp	0x4
     4f3: 94 24        	call	0x494
     4f4: 84 31        	movlp	0x4
     4f5: 77 08        	movf	0x77, w
     4f6: 20 00        	movlb	0x0
     4f7: ae 00        	movwf	0x2e
     4f8: 76 08        	movf	0x76, w
     4f9: ad 00        	movwf	0x2d
     4fa: 08 00        	return

Disassembly of section text4:

00000494 <__ptext4>:
     494: 78 30        	movlw	0x78
     495: f0 00        	movwf	0x70
     496: 76 08        	movf	0x76, w
     497: f1 00        	movwf	0x71
     498: 02 30        	movlw	0x2
     499: f2 00        	movwf	0x72
     49a: 85 31        	movlp	0x5
     49b: 7a 25        	call	0x57a
     49c: 84 31        	movlp	0x4
     49d: 79 08        	movf	0x79, w
     49e: f7 00        	movwf	0x77
     49f: 78 08        	movf	0x78, w
     4a0: f6 00        	movwf	0x76
     4a1: 08 00        	return

Disassembly of section text5:

0000057a <__ptext5>:
     57a: 70 08        	movf	0x70, w
     57b: f5 00        	movwf	0x75
     57c: 23 00        	movlb	0x3
     57d: 95 18        	btfsc	0x15, 0x1
     57e: 80 2d        	goto	0x580
     57f: 81 2d        	goto	0x581
     580: 7c 2d        	goto	0x57c
     581: 71 08        	movf	0x71, w
     582: f3 00        	movwf	0x73
     583: f4 01        	clrf	0x74
     584: 73 08        	movf	0x73, w
     585: 91 00        	movwf	0x11
     586: 74 08        	movf	0x74, w
     587: 92 00        	movwf	0x12
     588: 9b 2d        	goto	0x59b
     589: 95 18        	btfsc	0x15, 0x1
     58a: 8c 2d        	goto	0x58c
     58b: 8d 2d        	goto	0x58d
     58c: 89 2d        	goto	0x589
     58d: 7f 30        	movlw	0x7f
     58e: 95 05        	andwf	0x15, f
     58f: 15 14        	bsf	0x15, 0x0
     590: 75 08        	movf	0x75, w
     591: 86 00        	movwf	0x6
     592: 87 01        	clrf	0x7
     593: 13 08        	movf	0x13, w
     594: 81 00        	movwf	0x1
     595: 01 30        	movlw	0x1
     596: f5 07        	addwf	0x75, f
     597: 01 30        	movlw	0x1
     598: 91 07        	addwf	0x11, f
     599: 00 30        	movlw	0x0
     59a: 92 3d        	addwfc	0x12, f
     59b: 01 30        	movlw	0x1
     59c: f2 02        	subwf	0x72, f
     59d: 72 0a        	incf	0x72, w
     59e: 03 1d        	btfss	0x3, 0x2
     59f: a1 2d        	goto	0x5a1
     5a0: a2 2d        	goto	0x5a2
     5a1: 89 2d        	goto	0x589
     5a2: 08 00        	return

Disassembly of section text6:

00000487 <_btn_init>:
     487: fc 30        	movlw	0xfc
     488: 20 00        	movlb	0x0
     489: 9c 05        	andwf	0x1c, f
     48a: 64 30        	movlw	0x64
     48b: 9b 00        	movwf	0x1b
     48c: 1c 08        	movf	0x1c, w
     48d: 87 39        	andlw	0x87
     48e: 48 38        	iorlw	0x48
     48f: 9c 00        	movwf	0x1c
     490: 1c 15        	bsf	0x1c, 0x2
     491: 21 00        	movlb	0x1
     492: 91 14        	bsf	0x11, 0x1
     493: 08 00        	return

Disassembly of section text7:

00000717 <_bscan_scan>:
     717: 20 00        	movlb	0x0
     718: d5 01        	clrf	0x55
     719: 98 30        	movlw	0x98
     71a: e9 00        	movwf	0x69
     71b: 23 30        	movlw	0x23
     71c: ea 00        	movwf	0x6a
     71d: 67 30        	movlw	0x67
     71e: f4 00        	movwf	0x74
     71f: 00 30        	movlw	0x0
     720: f5 00        	movwf	0x75
     721: 86 31        	movlp	0x6
     722: 79 26        	call	0x679
     723: 87 31        	movlp	0x7
     724: 63 30        	movlw	0x63
     725: f4 00        	movwf	0x74
     726: 00 30        	movlw	0x0
     727: f5 00        	movwf	0x75
     728: 86 31        	movlp	0x6
     729: 79 26        	call	0x679
     72a: 87 31        	movlp	0x7
     72b: 8f 2f        	goto	0x78f
     72c: 98 30        	movlw	0x98
     72d: e9 00        	movwf	0x69
     72e: 23 30        	movlw	0x23
     72f: ea 00        	movwf	0x6a
     730: 67 30        	movlw	0x67
     731: f4 00        	movwf	0x74
     732: 00 30        	movlw	0x0
     733: f5 00        	movwf	0x75
     734: 86 31        	movlp	0x6
     735: 79 26        	call	0x679
     736: 87 31        	movlp	0x7
     737: 63 30        	movlw	0x63
     738: f4 00        	movwf	0x74
     739: 00 30        	movlw	0x0
     73a: f5 00        	movwf	0x75
     73b: 86 31        	movlp	0x6
     73c: 79 26        	call	0x679
     73d: 87 31        	movlp	0x7
     73e: a0 30        	movlw	0xa0
     73f: 20 00        	movlb	0x0
     740: e9 00        	movwf	0x69
     741: 23 30        	movlw	0x23
     742: ea 00        	movwf	0x6a
     743: 67 30        	movlw	0x67
     744: f4 00        	movwf	0x74
     745: 00 30        	movlw	0x0
     746: f5 00        	movwf	0x75
     747: 86 31        	movlp	0x6
     748: 79 26        	call	0x679
     749: 87 31        	movlp	0x7
     74a: 63 30        	movlw	0x63
     74b: f4 00        	movwf	0x74
     74c: 00 30        	movlw	0x0
     74d: f5 00        	movwf	0x75
     74e: 86 31        	movlp	0x6
     74f: 79 26        	call	0x679
     750: 87 31        	movlp	0x7
     751: 20 00        	movlb	0x0
     752: d7 01        	clrf	0x57
     753: d8 01        	clrf	0x58
     754: 58 08        	movf	0x58, w
     755: a0 3e        	addlw	0x20, f
     756: 86 00        	movwf	0x6
     757: 87 01        	clrf	0x7
     758: 01 08        	movf	0x1, w
     759: d6 00        	movwf	0x56
     75a: 56 08        	movf	0x56, w
     75b: 84 31        	movlp	0x4
     75c: 4b 24        	call	0x44b
     75d: 87 31        	movlp	0x7
     75e: 20 00        	movlb	0x0
     75f: d4 00        	movwf	0x54
     760: 54 08        	movf	0x54, w
     761: 03 1d        	btfss	0x3, 0x2
     762: 64 2f        	goto	0x764
     763: 65 2f        	goto	0x765
     764: 66 2f        	goto	0x766
     765: 7f 2f        	goto	0x77f
     766: 57 08        	movf	0x57, w
     767: fc 00        	movwf	0x7c
     768: 56 08        	movf	0x56, w
     769: 86 31        	movlp	0x6
     76a: 05 26        	call	0x605
     76b: 87 31        	movlp	0x7
     76c: 20 00        	movlb	0x0
     76d: d3 00        	movwf	0x53
     76e: 53 08        	movf	0x53, w
     76f: 03 1d        	btfss	0x3, 0x2
     770: 72 2f        	goto	0x772
     771: 73 2f        	goto	0x773
     772: 75 2f        	goto	0x775
     773: d5 01        	clrf	0x55
     774: 87 2f        	goto	0x787
     775: 21 00        	movlb	0x1
     776: 56 08        	movf	0x56, w
     777: 20 00        	movlb	0x0
     778: 57 06        	xorwf	0x57, w
     779: 03 1d        	btfss	0x3, 0x2
     77a: 7c 2f        	goto	0x77c
     77b: 7d 2f        	goto	0x77d
     77c: 7f 2f        	goto	0x77f
     77d: 01 30        	movlw	0x1
     77e: d7 07        	addwf	0x57, f
     77f: 01 30        	movlw	0x1
     780: d8 07        	addwf	0x58, f
     781: 09 30        	movlw	0x9
     782: 58 02        	subwf	0x58, w
     783: 03 1c        	btfss	0x3, 0x0
     784: 86 2f        	goto	0x786
     785: 87 2f        	goto	0x787
     786: 54 2f        	goto	0x754
     787: 02 30        	movlw	0x2
     788: 57 06        	xorwf	0x57, w
     789: 03 1d        	btfss	0x3, 0x2
     78a: 8c 2f        	goto	0x78c
     78b: 8d 2f        	goto	0x78d
     78c: 8f 2f        	goto	0x78f
     78d: 01 30        	movlw	0x1
     78e: d5 00        	movwf	0x55
     78f: 20 00        	movlb	0x0
     790: 55 08        	movf	0x55, w
     791: 03 19        	btfsc	0x3, 0x2
     792: 94 2f        	goto	0x794
     793: 95 2f        	goto	0x795
     794: 2c 2f        	goto	0x72c
     795: e0 30        	movlw	0xe0
     796: e9 00        	movwf	0x69
     797: 23 30        	movlw	0x23
     798: ea 00        	movwf	0x6a
     799: 67 30        	movlw	0x67
     79a: f4 00        	movwf	0x74
     79b: 00 30        	movlw	0x0
     79c: f5 00        	movwf	0x75
     79d: 86 31        	movlp	0x6
     79e: 79 26        	call	0x679
     79f: 87 31        	movlp	0x7
     7a0: 63 30        	movlw	0x63
     7a1: f4 00        	movwf	0x74
     7a2: 00 30        	movlw	0x0
     7a3: f5 00        	movwf	0x75
     7a4: 86 31        	movlp	0x6
     7a5: 79 26        	call	0x679
     7a6: 87 31        	movlp	0x7
     7a7: e8 30        	movlw	0xe8
     7a8: 20 00        	movlb	0x0
     7a9: e9 00        	movwf	0x69
     7aa: 23 30        	movlw	0x23
     7ab: ea 00        	movwf	0x6a
     7ac: 67 30        	movlw	0x67
     7ad: f4 00        	movwf	0x74
     7ae: 00 30        	movlw	0x0
     7af: f5 00        	movwf	0x75
     7b0: 86 31        	movlp	0x6
     7b1: 79 26        	call	0x679
     7b2: 87 31        	movlp	0x7
     7b3: 63 30        	movlw	0x63
     7b4: f4 00        	movwf	0x74
     7b5: 00 30        	movlw	0x0
     7b6: f5 00        	movwf	0x75
     7b7: 86 31        	movlp	0x6
     7b8: 79 26        	call	0x679
     7b9: 87 31        	movlp	0x7
     7ba: 08 00        	return

Disassembly of section text8:

00000605 <_bscan_scan_slot_rest>:
     605: 20 00        	movlb	0x0
     606: d0 00        	movwf	0x50
     607: 03 30        	movlw	0x3
     608: d2 00        	movwf	0x52
     609: 08 30        	movlw	0x8
     60a: 52 06        	xorwf	0x52, w
     60b: 03 1d        	btfss	0x3, 0x2
     60c: 0e 2e        	goto	0x60e
     60d: 0f 2e        	goto	0x60f
     60e: 24 2e        	goto	0x624
     60f: 52 08        	movf	0x52, w
     610: cc 00        	movwf	0x4c
     611: cd 01        	clrf	0x4d
     612: cc 35        	lslf	0x4c, f
     613: cd 0d        	rlf	0x4d, f
     614: cc 35        	lslf	0x4c, f
     615: cd 0d        	rlf	0x4d, f
     616: cc 35        	lslf	0x4c, f
     617: cd 0d        	rlf	0x4d, f
     618: 9a 30        	movlw	0x9a
     619: ce 00        	movwf	0x4e
     61a: 23 30        	movlw	0x23
     61b: cf 00        	movwf	0x4f
     61c: 4c 08        	movf	0x4c, w
     61d: 4e 07        	addwf	0x4e, w
     61e: 86 00        	movwf	0x6
     61f: 4d 08        	movf	0x4d, w
     620: 4f 3d        	addwfc	0x4f, w
     621: 87 00        	movwf	0x7
     622: 7c 08        	movf	0x7c, w
     623: 81 00        	movwf	0x1
     624: 52 08        	movf	0x52, w
     625: f6 00        	movwf	0x76
     626: 50 08        	movf	0x50, w
     627: 85 31        	movlp	0x5
     628: a3 25        	call	0x5a3
     629: 86 31        	movlp	0x6
     62a: 20 00        	movlb	0x0
     62b: d1 00        	movwf	0x51
     62c: 51 08        	movf	0x51, w
     62d: 03 1d        	btfss	0x3, 0x2
     62e: 30 2e        	goto	0x630
     62f: 31 2e        	goto	0x631
     630: 33 2e        	goto	0x633
     631: 00 30        	movlw	0x0
     632: 3c 2e        	goto	0x63c
     633: 01 30        	movlw	0x1
     634: d2 07        	addwf	0x52, f
     635: 09 30        	movlw	0x9
     636: 52 02        	subwf	0x52, w
     637: 03 1c        	btfss	0x3, 0x0
     638: 3a 2e        	goto	0x63a
     639: 3b 2e        	goto	0x63b
     63a: 09 2e        	goto	0x609
     63b: 01 30        	movlw	0x1
     63c: 08 00        	return

Disassembly of section text9:

0000044b <_bscan_scan_slot_first>:
     44b: fc 00        	movwf	0x7c
     44c: 02 30        	movlw	0x2
     44d: f6 00        	movwf	0x76
     44e: 7c 08        	movf	0x7c, w
     44f: 85 31        	movlp	0x5
     450: a3 25        	call	0x5a3
     451: 84 31        	movlp	0x4
     452: 08 00        	return

Disassembly of section text10:

000005a3 <_bscan_scan_slot_data>:
     5a3: fb 00        	movwf	0x7b
     5a4: 76 08        	movf	0x76, w
     5a5: f7 00        	movwf	0x77
     5a6: f8 01        	clrf	0x78
     5a7: f7 35        	lslf	0x77, f
     5a8: f8 0d        	rlf	0x78, f
     5a9: f7 35        	lslf	0x77, f
     5aa: f8 0d        	rlf	0x78, f
     5ab: f7 35        	lslf	0x77, f
     5ac: f8 0d        	rlf	0x78, f
     5ad: 98 30        	movlw	0x98
     5ae: f9 00        	movwf	0x79
     5af: 23 30        	movlw	0x23
     5b0: fa 00        	movwf	0x7a
     5b1: 77 08        	movf	0x77, w
     5b2: 79 07        	addwf	0x79, w
     5b3: 20 00        	movlb	0x0
     5b4: e9 00        	movwf	0x69
     5b5: 78 08        	movf	0x78, w
     5b6: 7a 3d        	addwfc	0x7a, w
     5b7: ea 00        	movwf	0x6a
     5b8: 69 08        	movf	0x69, w
     5b9: 86 00        	movwf	0x6
     5ba: 6a 08        	movf	0x6a, w
     5bb: 87 00        	movwf	0x7
     5bc: 7b 08        	movf	0x7b, w
     5bd: 81 00        	movwf	0x1
     5be: 67 30        	movlw	0x67
     5bf: f4 00        	movwf	0x74
     5c0: 00 30        	movlw	0x0
     5c1: f5 00        	movwf	0x75
     5c2: 86 31        	movlp	0x6
     5c3: 79 26        	call	0x679
     5c4: 85 31        	movlp	0x5
     5c5: 63 30        	movlw	0x63
     5c6: f4 00        	movwf	0x74
     5c7: 00 30        	movlw	0x0
     5c8: f5 00        	movwf	0x75
     5c9: 86 31        	movlp	0x6
     5ca: 79 26        	call	0x679
     5cb: 85 31        	movlp	0x5
     5cc: 20 00        	movlb	0x0
     5cd: 63 0c        	rrf	0x63, w
     5ce: 01 39        	andlw	0x1
     5cf: 08 00        	return

Disassembly of section text11:

00000679 <_lin_txrx_blocking>:
     679: 74 08        	movf	0x74, w
     67a: 86 00        	movwf	0x6
     67b: 75 08        	movf	0x75, w
     67c: 87 00        	movwf	0x7
     67d: 01 0c        	rrf	0x1, w
     67e: 89 0c        	rrf	0x9, f
     67f: 3f 39        	andlw	0x3f
     680: 20 00        	movlb	0x0
     681: e2 00        	movwf	0x62
     682: 74 08        	movf	0x74, w
     683: 86 00        	movwf	0x6
     684: 75 08        	movf	0x75, w
     685: 87 00        	movwf	0x7
     686: 41 31        	addfsr	1, 0x1
     687: 01 08        	movf	0x1, w
     688: e1 00        	movwf	0x61
     689: 74 08        	movf	0x74, w
     68a: 86 00        	movwf	0x6
     68b: 75 08        	movf	0x75, w
     68c: 87 00        	movwf	0x7
     68d: 42 31        	addfsr	1, 0x2
     68e: 40 3f        	moviw	0[FSR1]
     68f: d9 00        	movwf	0x59
     690: 41 3f        	moviw	1[FSR1]
     691: da 00        	movwf	0x5a
     692: 74 08        	movf	0x74, w
     693: 86 00        	movwf	0x6
     694: 75 08        	movf	0x75, w
     695: 87 00        	movwf	0x7
     696: 01 1c        	btfss	0x1, 0x0
     697: 99 2e        	goto	0x699
     698: 9a 2e        	goto	0x69a
     699: 9e 2e        	goto	0x69e
     69a: 84 31        	movlp	0x4
     69b: 65 24        	call	0x465
     69c: 86 31        	movlp	0x6
     69d: a1 2e        	goto	0x6a1
     69e: 84 31        	movlp	0x4
     69f: 5b 24        	call	0x45b
     6a0: 86 31        	movlp	0x6
     6a1: 28 00        	movlb	0x8
     6a2: 95 01        	clrf	0x15
     6a3: 20 00        	movlb	0x0
     6a4: 93 10        	bcf	0x13, 0x1
     6a5: 64 00        	tris	0x4
     6a6: 20 00        	movlb	0x0
     6a7: 93 1c        	btfss	0x13, 0x1
     6a8: aa 2e        	goto	0x6aa
     6a9: ab 2e        	goto	0x6ab
     6aa: a6 2e        	goto	0x6a6
     6ab: 93 10        	bcf	0x13, 0x1
     6ac: be 1c        	btfss	0x3e, 0x1
     6ad: af 2e        	goto	0x6af
     6ae: b0 2e        	goto	0x6b0
     6af: b9 2e        	goto	0x6b9
     6b0: 74 08        	movf	0x74, w
     6b1: 86 00        	movwf	0x6
     6b2: 75 08        	movf	0x75, w
     6b3: 87 00        	movwf	0x7
     6b4: 81 10        	bcf	0x1, 0x1
     6b5: 84 31        	movlp	0x4
     6b6: 39 24        	call	0x439
     6b7: 86 31        	movlp	0x6
     6b8: be 2e        	goto	0x6be
     6b9: 74 08        	movf	0x74, w
     6ba: 86 00        	movwf	0x6
     6bb: 75 08        	movf	0x75, w
     6bc: 87 00        	movwf	0x7
     6bd: 81 14        	bsf	0x1, 0x1
     6be: 08 00        	return

Disassembly of section text12:

00000465 <_lin_tx_frame>:
     465: 20 00        	movlb	0x0
     466: be 18        	btfsc	0x3e, 0x1
     467: 69 2c        	goto	0x469
     468: 6a 2c        	goto	0x46a
     469: 6e 2c        	goto	0x46e
     46a: 3e 14        	bsf	0x3e, 0x0
     46b: 85 31        	movlp	0x5
     46c: 13 25        	call	0x513
     46d: 84 31        	movlp	0x4
     46e: 08 00        	return

Disassembly of section text13:

0000045b <_lin_rx_frame>:
     45b: 20 00        	movlb	0x0
     45c: be 18        	btfsc	0x3e, 0x1
     45d: 5f 2c        	goto	0x45f
     45e: 60 2c        	goto	0x460
     45f: 64 2c        	goto	0x464
     460: 3e 10        	bcf	0x3e, 0x0
     461: 85 31        	movlp	0x5
     462: 13 25        	call	0x513
     463: 84 31        	movlp	0x4
     464: 08 00        	return

Disassembly of section text14:

00000513 <_lin_txrx>:
     513: 20 00        	movlb	0x0
     514: be 14        	bsf	0x3e, 0x1
     515: 62 08        	movf	0x62, w
     516: 85 31        	movlp	0x5
     517: 2b 25        	call	0x52b
     518: 85 31        	movlp	0x5
     519: 20 00        	movlb	0x0
     51a: b6 00        	movwf	0x36
     51b: 3e 1c        	btfss	0x3e, 0x0
     51c: 1e 2d        	goto	0x51e
     51d: 1f 2d        	goto	0x51f
     51e: 24 2d        	goto	0x524
     51f: 86 31        	movlp	0x6
     520: 3d 26        	call	0x63d
     521: 85 31        	movlp	0x5
     522: 20 00        	movlb	0x0
     523: b5 00        	movwf	0x35
     524: 23 00        	movlb	0x3
     525: 9e 15        	bsf	0x1e, 0x3
     526: 9a 01        	clrf	0x1a
     527: 01 30        	movlw	0x1
     528: 20 00        	movlb	0x0
     529: bf 00        	movwf	0x3f
     52a: 08 00        	return

Disassembly of section text15:

0000052b <_lin_compute_protected_id>:
     52b: f1 00        	movwf	0x71
     52c: 71 08        	movf	0x71, w
     52d: f3 00        	movwf	0x73
     52e: 71 08        	movf	0x71, w
     52f: f2 00        	movwf	0x72
     530: 03 10        	bcf	0x3, 0x0
     531: f3 0d        	rlf	0x73, f
     532: 73 08        	movf	0x73, w
     533: f2 06        	xorwf	0x72, f
     534: 03 10        	bcf	0x3, 0x0
     535: f3 0d        	rlf	0x73, f
     536: 73 08        	movf	0x73, w
     537: f2 06        	xorwf	0x72, f
     538: 73 1f        	btfss	0x73, 0x6
     539: 3b 2d        	goto	0x53b
     53a: 3c 2d        	goto	0x53c
     53b: 3d 2d        	goto	0x53d
     53c: 73 14        	bsf	0x73, 0x0
     53d: 03 10        	bcf	0x3, 0x0
     53e: f3 0d        	rlf	0x73, f
     53f: 03 10        	bcf	0x3, 0x0
     540: f3 0d        	rlf	0x73, f
     541: 73 08        	movf	0x73, w
     542: f2 06        	xorwf	0x72, f
     543: 71 08        	movf	0x71, w
     544: 3f 39        	andlw	0x3f
     545: f0 00        	movwf	0x70
     546: 72 1d        	btfss	0x72, 0x2
     547: 49 2d        	goto	0x549
     548: 4a 2d        	goto	0x54a
     549: 4b 2d        	goto	0x54b
     54a: 70 17        	bsf	0x70, 0x6
     54b: f2 1a        	btfsc	0x72, 0x5
     54c: 4e 2d        	goto	0x54e
     54d: 4f 2d        	goto	0x54f
     54e: 50 2d        	goto	0x550
     54f: f0 17        	bsf	0x70, 0x7
     550: 70 08        	movf	0x70, w
     551: 08 00        	return

Disassembly of section text16:

0000063d <_lin_compute_checksum>:
     63d: f3 01        	clrf	0x73
     63e: 3c 30        	movlw	0x3c
     63f: 20 00        	movlb	0x0
     640: 62 06        	xorwf	0x62, w
     641: 03 19        	btfsc	0x3, 0x2
     642: 44 2e        	goto	0x644
     643: 45 2e        	goto	0x645
     644: 4d 2e        	goto	0x64d
     645: 3d 30        	movlw	0x3d
     646: 62 06        	xorwf	0x62, w
     647: 03 19        	btfsc	0x3, 0x2
     648: 4a 2e        	goto	0x64a
     649: 4b 2e        	goto	0x64b
     64a: 4d 2e        	goto	0x64d
     64b: 36 08        	movf	0x36, w
     64c: f3 07        	addwf	0x73, f
     64d: f2 01        	clrf	0x72
     64e: 6f 2e        	goto	0x66f
     64f: 72 08        	movf	0x72, w
     650: 59 07        	addwf	0x59, w
     651: 86 00        	movwf	0x6
     652: 00 30        	movlw	0x0
     653: 5a 3d        	addwfc	0x5a, w
     654: 87 00        	movwf	0x7
     655: 73 08        	movf	0x73, w
     656: 01 07        	addwf	0x1, w
     657: f0 00        	movwf	0x70
     658: f1 01        	clrf	0x71
     659: f1 0d        	rlf	0x71, f
     65a: 72 08        	movf	0x72, w
     65b: 59 07        	addwf	0x59, w
     65c: 86 00        	movwf	0x6
     65d: 00 30        	movlw	0x0
     65e: 5a 3d        	addwfc	0x5a, w
     65f: 87 00        	movwf	0x7
     660: 01 08        	movf	0x1, w
     661: f3 07        	addwf	0x73, f
     662: 01 30        	movlw	0x1
     663: 71 02        	subwf	0x71, w
     664: 00 30        	movlw	0x0
     665: 03 19        	btfsc	0x3, 0x2
     666: 70 02        	subwf	0x70, w
     667: 03 1c        	btfss	0x3, 0x0
     668: 6a 2e        	goto	0x66a
     669: 6b 2e        	goto	0x66b
     66a: 6d 2e        	goto	0x66d
     66b: 01 30        	movlw	0x1
     66c: f3 07        	addwf	0x73, f
     66d: 01 30        	movlw	0x1
     66e: f2 07        	addwf	0x72, f
     66f: 61 08        	movf	0x61, w
     670: 72 02        	subwf	0x72, w
     671: 03 1c        	btfss	0x3, 0x0
     672: 74 2e        	goto	0x674
     673: 75 2e        	goto	0x675
     674: 4f 2e        	goto	0x64f
     675: 73 09        	comf	0x73, w
     676: f3 00        	movwf	0x73
     677: 73 08        	movf	0x73, w
     678: 08 00        	return

Disassembly of section text17:

00000439 <_lin_reset_frame>:
     439: 20 00        	movlb	0x0
     43a: e1 01        	clrf	0x61
     43b: be 01        	clrf	0x3e
     43c: b3 01        	clrf	0x33
     43d: 08 00        	return

Disassembly of section text18:

000004b1 <_bscan_init>:
     4b1: 28 00        	movlb	0x8
     4b2: 17 08        	movf	0x17, w
     4b3: fc 39        	andlw	0xfc
     4b4: 02 38        	iorlw	0x2
     4b5: 97 00        	movwf	0x17
     4b6: fa 30        	movlw	0xfa
     4b7: 96 00        	movwf	0x16
     4b8: 17 08        	movf	0x17, w
     4b9: 87 39        	andlw	0x87
     4ba: 48 38        	iorlw	0x48
     4bb: 97 00        	movwf	0x17
     4bc: 21 00        	movlb	0x1
     4bd: 93 10        	bcf	0x13, 0x1
     4be: 28 00        	movlb	0x8
     4bf: 17 15        	bsf	0x17, 0x2
     4c0: 08 00        	return

Disassembly of section text19:

000004a2 <_bctrl_init>:
     4a2: 28 00        	movlb	0x8
     4a3: 17 08        	movf	0x17, w
     4a4: fc 39        	andlw	0xfc
     4a5: 02 38        	iorlw	0x2
     4a6: 97 00        	movwf	0x17
     4a7: 7d 30        	movlw	0x7d
     4a8: 96 00        	movwf	0x16
     4a9: 17 08        	movf	0x17, w
     4aa: 87 39        	andlw	0x87
     4ab: 48 38        	iorlw	0x48
     4ac: 97 00        	movwf	0x17
     4ad: 17 15        	bsf	0x17, 0x2
     4ae: 21 00        	movlb	0x1
     4af: 93 14        	bsf	0x13, 0x1
     4b0: 08 00        	return

Disassembly of section text20:

000004c1 <__ptext20>:
     4c1: 00 30        	movlw	0x0
     4c2: 20 00        	movlb	0x0
     4c3: df 00        	movwf	0x5f
     4c4: 00 30        	movlw	0x0
     4c5: e0 00        	movwf	0x60
     4c6: 00 30        	movlw	0x0
     4c7: dd 00        	movwf	0x5d
     4c8: 00 30        	movlw	0x0
     4c9: de 00        	movwf	0x5e
     4ca: 00 30        	movlw	0x0
     4cb: db 00        	movwf	0x5b
     4cc: 00 30        	movlw	0x0
     4cd: dc 00        	movwf	0x5c
     4ce: 84 31        	movlp	0x4
     4cf: fb 24        	call	0x4fb
     4d0: 84 31        	movlp	0x4
     4d1: 08 00        	return

Disassembly of section text21:

000004fb <__ptext21>:
     4fb: 23 00        	movlb	0x3
     4fc: 9d 13        	bcf	0x1d, 0x7
     4fd: 1e 12        	bcf	0x1e, 0x4
     4fe: 1e 15        	bsf	0x1e, 0x2
     4ff: 9f 15        	bsf	0x1f, 0x3
     500: cf 30        	movlw	0xcf
     501: 9b 00        	movwf	0x1b
     502: 9c 01        	clrf	0x1c
     503: 9f 10        	bcf	0x1f, 0x1
     504: 1f 10        	bcf	0x1f, 0x0
     505: 9e 11        	bcf	0x1e, 0x3
     506: 9d 11        	bcf	0x1d, 0x3
     507: 1d 16        	bsf	0x1d, 0x4
     508: 9e 12        	bcf	0x1e, 0x5
     509: 20 00        	movlb	0x0
     50a: 91 12        	bcf	0x11, 0x5
     50b: 21 00        	movlb	0x1
     50c: 91 16        	bsf	0x11, 0x5
     50d: 0b 17        	bsf	0xb, 0x6
     50e: 8b 17        	bsf	0xb, 0x7
     50f: 23 00        	movlb	0x3
     510: 1f 16        	bsf	0x1f, 0x4
     511: 9d 17        	bsf	0x1d, 0x7
     512: 08 00        	return

Disassembly of section text22:

000004d2 <__ptext22>:
     4d2: 78 30        	movlw	0x78
     4d3: 21 00        	movlb	0x1
     4d4: 99 04        	iorwf	0x19, f
     4d5: 03 30        	movlw	0x3
     4d6: 99 04        	iorwf	0x19, f
     4d7: 17 08        	movf	0x17, w
     4d8: c1 39        	andlw	0xc1
     4d9: 0a 38        	iorlw	0xa
     4da: 97 00        	movwf	0x17
     4db: 17 14        	bsf	0x17, 0x0
     4dc: 95 17        	bsf	0x15, 0x7
     4dd: 15 17        	bsf	0x15, 0x6
     4de: 95 12        	bcf	0x15, 0x5
     4df: 15 16        	bsf	0x15, 0x4
     4e0: 95 11        	bcf	0x15, 0x3
     4e1: 15 08        	movf	0x15, w
     4e2: f8 39        	andlw	0xf8
     4e3: 02 38        	iorlw	0x2
     4e4: 95 00        	movwf	0x15
     4e5: 08 00        	return

Disassembly of section maintext:

00000552 <_main>:
     552: 84 31        	movlp	0x4
     553: d2 24        	call	0x4d2
     554: 85 31        	movlp	0x5
     555: 84 31        	movlp	0x4
     556: c1 24        	call	0x4c1
     557: 85 31        	movlp	0x5
     558: 23 00        	movlb	0x3
     559: 9e 16        	bsf	0x1e, 0x5
     55a: 8d 01        	clrf	0xd
     55b: 03 30        	movlw	0x3
     55c: 21 00        	movlb	0x1
     55d: 8d 00        	movwf	0xd
     55e: 01 30        	movlw	0x1
     55f: 22 00        	movlb	0x2
     560: 8e 00        	movwf	0xe
     561: 80 30        	movlw	0x80
     562: 21 00        	movlb	0x1
     563: 8e 00        	movwf	0xe
     564: 84 31        	movlp	0x4
     565: 44 24        	call	0x444
     566: 85 31        	movlp	0x5
     567: 84 31        	movlp	0x4
     568: b1 24        	call	0x4b1
     569: 85 31        	movlp	0x5
     56a: 87 31        	movlp	0x7
     56b: 17 27        	call	0x717
     56c: 85 31        	movlp	0x5
     56d: 84 31        	movlp	0x4
     56e: a2 24        	call	0x4a2
     56f: 85 31        	movlp	0x5
     570: 84 31        	movlp	0x4
     571: e6 24        	call	0x4e6
     572: 85 31        	movlp	0x5
     573: 84 31        	movlp	0x4
     574: 87 24        	call	0x487
     575: 85 31        	movlp	0x5
     576: 64 00        	tris	0x4
     577: 76 2d        	goto	0x576
     578: 80 31        	movlp	0x0
     579: 02 28        	goto	0x2

Disassembly of section inittext:

0000047b <init_ram>:
     47b: fe 00        	movwf	0x7e
     47c: 12 00        	moviw	FSR0++
     47d: 1e 00        	movwi	FSR1++
     47e: 01 30        	movlw	0x1
     47f: fe 02        	subwf	0x7e, f
     480: 00 30        	movlw	0x0
     481: ff 3b        	<unknown>
     482: 7f 08        	movf	0x7f, w
     483: 7e 04        	iorwf	0x7e, w
     484: 03 1d        	btfss	0x3, 0x2
     485: 7c 2c        	goto	0x47c
     486: 00 34        	retlw	0x0

Disassembly of section reset_vec:

00000000 <reset_vec>:
       0: 80 31        	movlp	0x0
       1: 02 28        	goto	0x2
       2: 87 31        	movlp	0x7
       3: bb 2f        	goto	0x7bb

Disassembly of section clrtext:

0000046f <clear_ram0>:
     46f: 64 00        	tris	0x4
     470: 80 01        	clrf	0x0
     471: 01 31        	addfsr	0, 0x1
     472: 01 30        	movlw	0x1
     473: fe 02        	subwf	0x7e, f
     474: 00 30        	movlw	0x0
     475: ff 3b        	<unknown>
     476: 7f 08        	movf	0x7f, w
     477: 7e 04        	iorwf	0x7e, w
     478: 03 1d        	btfss	0x3, 0x2
     479: 70 2c        	goto	0x470
     47a: 00 34        	retlw	0x0
