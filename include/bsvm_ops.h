/* Autogenerated header */

#define BSVM_SOP_NOP			0
#define BSVM_SOP_BLOCK			1
#define BSVM_SOP_DBGMARK			2
#define BSVM_SOP_CGENAST			3
#define BSVM_SOP_FN			4
#define BSVM_SOP_LN			5
#define BSVM_SOP_LABEL			6
#define BSVM_SOP_PUSH			16
#define BSVM_SOP_POP			17
#define BSVM_SOP_LOAD			18
#define BSVM_SOP_STORE			19
#define BSVM_SOP_BIND			20
#define BSVM_SOP_DYNBIND			21
#define BSVM_SOP_LEXBIND			22
#define BSVM_SOP_LOADINDEX		23
#define BSVM_SOP_STOREINDEX		24
#define BSVM_SOP_PUSH_SELF		25
#define BSVM_SOP_CLEARENV		26
#define BSVM_SOP_TRYCAST_S		27
#define BSVM_SOP_PF_HINT_PTR		28
#define BSVM_SOP_PF_HINT_S		29
#define BSVM_SOP_TRYLOAD			30
#define BSVM_SOP_CAST_S			31
#define BSVM_SOP_MARK			32
#define BSVM_SOP_CALL			33
#define BSVM_SOP_TAILCALL		34
#define BSVM_SOP_CALL_S			35
#define BSVM_SOP_TAILCALL_S		36
#define BSVM_SOP_METHODCALL		37
#define BSVM_SOP_METHODTAILCALL		38
#define BSVM_SOP_METHODCALL_S		39
#define BSVM_SOP_METHODTAILCALL_S	40
#define BSVM_SOP_RET			41
#define BSVM_SOP_TYMARK			42
#define BSVM_SOP_NEW_S			47
#define BSVM_SOP_DUP			48
#define BSVM_SOP_EXCH			49
#define BSVM_SOP_INDEX			50
#define BSVM_SOP_RINDEX			51
#define BSVM_SOP_SETINDEX		52
#define BSVM_SOP_SETRINDEX		53
#define BSVM_SOP_DUP_F			54
#define BSVM_SOP_EXCH_SELF		60
#define BSVM_SOP_PUSH_CTV		61
#define BSVM_SOP_DELETE			62
#define BSVM_SOP_CAST			63
#define BSVM_SOP_UNARYOP			64
#define BSVM_SOP_BINARYOP		65
#define BSVM_SOP_PUSH_NULL		66
#define BSVM_SOP_PUSH_TRUE		67
#define BSVM_SOP_PUSH_FALSE		68
#define BSVM_SOP_PUSH_SV			69
#define BSVM_SOP_PUSH_0			70
#define BSVM_SOP_PUSH_1			71
#define BSVM_SOP_PUSH_2			72
#define BSVM_SOP_PUSH_3			73
#define BSVM_SOP_PUSH_4			74
#define BSVM_SOP_PUSH_5			75
#define BSVM_SOP_PUSH_6			76
#define BSVM_SOP_PUSH_7			77
#define BSVM_SOP_PUSH_CF			78
#define BSVM_SOP_CALL_CF			79
#define BSVM_SOP_TAILCALL_CF		80
#define BSVM_SOP_CAR			81
#define BSVM_SOP_CDR			82
#define BSVM_SOP_CAAR			83
#define BSVM_SOP_CDAR			84
#define BSVM_SOP_CADR			85
#define BSVM_SOP_CDDR			86
#define BSVM_SOP_PUSH_SV_P		87
#define BSVM_SOP_PUSH_SV_N		88
#define BSVM_SOP_PUSH_SV_F		89
#define BSVM_SOP_LLOAD			90
#define BSVM_SOP_LSTORE			91
#define BSVM_SOP_LLOAD_F			92
#define BSVM_SOP_LSTORE_F		93
#define BSVM_SOP_LPOSTINC		94
#define BSVM_SOP_LPOSTDEC		95
#define BSVM_SOP_LINC			96
#define BSVM_SOP_LDEC			97
#define BSVM_SOP_LPREINC			98
#define BSVM_SOP_LPREDEC			99
#define BSVM_SOP_LOADINDEX_S		100
#define BSVM_SOP_STOREINDEX_S		101
#define BSVM_SOP_LOADINDEX_I		102
#define BSVM_SOP_STOREINDEX_I		103
#define BSVM_SOP_LOADINDEX_0		104
#define BSVM_SOP_LOADINDEX_1		105
#define BSVM_SOP_LOADINDEX_2		106
#define BSVM_SOP_LOADINDEX_3		107
#define BSVM_SOP_STOREINDEX_0		108
#define BSVM_SOP_STOREINDEX_1		109
#define BSVM_SOP_STOREINDEX_2		110
#define BSVM_SOP_STOREINDEX_3		111
#define BSVM_SOP_THROW_S			112
#define BSVM_SOP_BEGIN_TRY		113
#define BSVM_SOP_END_TRY			114
#define BSVM_SOP_CATCH			115
#define BSVM_SOP_THROW_OBJ		116
#define BSVM_SOP_PF_WIDE			117
#define BSVM_SOP_JMP			118
#define BSVM_SOP_JMP_TRUE		119
#define BSVM_SOP_JMP_FALSE		120
#define BSVM_SOP_JMP_COND_U		121
#define BSVM_SOP_JMP_COND_B		122
#define BSVM_SOP_BEGIN_CATCH		123
#define BSVM_SOP_END_CATCH		124
#define BSVM_SOP_BEGIN_CATCH_FINAL	125
#define BSVM_SOP_ARRAY			128
#define BSVM_SOP_DICT			129
#define BSVM_SOP_ATTR			130
#define BSVM_SOP_OBJECT			131
#define BSVM_SOP_VECTOR			132
#define BSVM_SOP_TERMINAL		133
#define BSVM_SOP_COMPLEX			134
#define BSVM_SOP_MATRIX			135
#define BSVM_SOP_LIST			136
#define BSVM_SOP_COMPLEX_I		137
#define BSVM_SOP_CLOSE			138
#define BSVM_SOP_CLOSE2			139
#define BSVM_SOP_FLINK			140
#define BSVM_SOP_CONS			141
#define BSVM_SOP_QUAT			142
#define BSVM_SOP_ENVCLOSE		143
#define BSVM_SOP_NEG			144
#define BSVM_SOP_LNOT			145
#define BSVM_SOP_DEFER			146
#define BSVM_SOP_PUSH_SV_C		147
#define BSVM_SOP_ARRAY_S			148
#define BSVM_SOP_CLONE			149
#define BSVM_SOP_TOSTRING		150
#define BSVM_SOP_TYBIND			151
#define BSVM_SOP_TYDYNBIND		152
#define BSVM_SOP_TYLEXBIND		153
#define BSVM_SOP_IMPORT			154
#define BSVM_SOP_LOADA_S			155
#define BSVM_SOP_LOADINDEXA		156
#define BSVM_SOP_CLASS_IFACE		157
#define BSVM_SOP_INSTANCEOF_S		158
#define BSVM_SOP_ADD			159
#define BSVM_SOP_SUB			160
#define BSVM_SOP_MUL			161
#define BSVM_SOP_DIV			162
#define BSVM_SOP_MOD			163
#define BSVM_SOP_AND			164
#define BSVM_SOP_OR			165
#define BSVM_SOP_XOR			166
#define BSVM_SOP_POSTINC_S		167
#define BSVM_SOP_POSTDEC_S		168
#define BSVM_SOP_PREINC_S		169
#define BSVM_SOP_PREDEC_S		170
#define BSVM_SOP_INC_S			171
#define BSVM_SOP_DEC_S			172
#define BSVM_SOP_SETDEFER		173
#define BSVM_SOP_NOT			174
#define BSVM_SOP_INC			175
#define BSVM_SOP_DEC			176
#define BSVM_SOP_PUSH_SV_FB		177
#define BSVM_SOP_ROLL_I			178
#define BSVM_SOP_COPY_I			179
#define BSVM_SOP_PF_HINT_XI		180
#define BSVM_SOP_PF_HINT_XL		181
#define BSVM_SOP_PF_HINT_XF		182
#define BSVM_SOP_PF_HINT_XD		183
#define BSVM_SOP_PUSH_XI			184
#define BSVM_SOP_PUSH_XL			185
#define BSVM_SOP_SHL			186
#define BSVM_SOP_SHR			187
#define BSVM_SOP_INC2			188
#define BSVM_SOP_DEC2			189
#define BSVM_SOP_JMP_L_FN		256
#define BSVM_SOP_JMP_G_FN		257
#define BSVM_SOP_JMP_LE_FN		258
#define BSVM_SOP_JMP_GE_FN		259
#define BSVM_SOP_JMP_L_FL		260
#define BSVM_SOP_JMP_G_FL		261
#define BSVM_SOP_JMP_LE_FL		262
#define BSVM_SOP_JMP_GE_FL		263
#define BSVM_SOP_JMP_E_LFN		264
#define BSVM_SOP_JMP_NE_LFN		265
#define BSVM_SOP_JMP_L_LFN		266
#define BSVM_SOP_JMP_G_LFN		267
#define BSVM_SOP_JMP_LE_LFN		268
#define BSVM_SOP_JMP_GE_LFN		269
#define BSVM_SOP_JMP_E_ZFN		270
#define BSVM_SOP_JMP_NE_ZFN		271
#define BSVM_SOP_JMP_L_ZFN		272
#define BSVM_SOP_JMP_G_ZFN		273
#define BSVM_SOP_JMP_LE_ZFN		274
#define BSVM_SOP_JMP_GE_ZFN		275
#define BSVM_SOP_JMP_E_LZFN		276
#define BSVM_SOP_JMP_NE_LZFN		277
#define BSVM_SOP_JMP_L_LZFN		278
#define BSVM_SOP_JMP_G_LZFN		279
#define BSVM_SOP_JMP_LE_LZFN		280
#define BSVM_SOP_JMP_GE_LZFN		281
#define BSVM_SOP_JMP_E_FNC		282
#define BSVM_SOP_JMP_NE_FNC		283
#define BSVM_SOP_JMP_L_FNC		284
#define BSVM_SOP_JMP_G_FNC		285
#define BSVM_SOP_JMP_LE_FNC		286
#define BSVM_SOP_JMP_GE_FNC		287
#define BSVM_SOP_JMP_E_LFNC		288
#define BSVM_SOP_JMP_NE_LFNC		289
#define BSVM_SOP_JMP_L_LFNC		290
#define BSVM_SOP_JMP_G_LFNC		291
#define BSVM_SOP_JMP_LE_LFNC		292
#define BSVM_SOP_JMP_GE_LFNC		293
#define BSVM_SOP_INC_FN			294
#define BSVM_SOP_DEC_FN			295
#define BSVM_SOP_INC2_FN			296
#define BSVM_SOP_DEC2_FN			297
#define BSVM_SOP_ADD_FN			298
#define BSVM_SOP_SUB_FN			299
#define BSVM_SOP_MUL_FN			300
#define BSVM_SOP_DIV_FN			301
#define BSVM_SOP_IDIV_FN			302
#define BSVM_SOP_MOD_FN			303
#define BSVM_SOP_AND_FN			304
#define BSVM_SOP_OR_FN			305
#define BSVM_SOP_XOR_FN			306
#define BSVM_SOP_CMP_L_FN		307
#define BSVM_SOP_CMP_G_FN		308
#define BSVM_SOP_CMP_LE_FN		309
#define BSVM_SOP_CMP_GE_FN		310
#define BSVM_SOP_CMP_E_FN		311
#define BSVM_SOP_CMP_NE_FN		312
#define BSVM_SOP_NEG_FN			313
#define BSVM_SOP_NOT_FN			314
#define BSVM_SOP_ADD_FN_C		315
#define BSVM_SOP_SUB_FN_C		316
#define BSVM_SOP_MUL_FN_C		317
#define BSVM_SOP_CONV_FN2FL		318
#define BSVM_SOP_CONV_FL2FN		319
#define BSVM_SOP_ADD_FL			320
#define BSVM_SOP_SUB_FL			321
#define BSVM_SOP_MUL_FL			322
#define BSVM_SOP_DIV_FL			323
#define BSVM_SOP_CMP_L_FL		324
#define BSVM_SOP_CMP_G_FL		325
#define BSVM_SOP_CMP_LE_FL		326
#define BSVM_SOP_CMP_GE_FL		327
#define BSVM_SOP_CMP_E_FL		328
#define BSVM_SOP_CMP_NE_FL		329
#define BSVM_SOP_NEG_FL			330
#define BSVM_SOP_JMP_E_FN		331
#define BSVM_SOP_JMP_NE_FN		332
#define BSVM_SOP_JMP_E_FL		333
#define BSVM_SOP_JMP_NE_FL		334
#define BSVM_SOP_SHL_FN			335
#define BSVM_SOP_SHR_FN			336
#define BSVM_SOP_EXP_FN			337
#define BSVM_SOP_SHL_FN_C		338
#define BSVM_SOP_SHR_FN_C		339
#define BSVM_SOP_EXP_FN_C		340
#define BSVM_SOP_PUSH_XF			341
#define BSVM_SOP_PUSH_XD			342
#define BSVM_SOP_SHRR			344
#define BSVM_SOP_LOADB_S			345
#define BSVM_SOP_LOADINDEXA_S		346
#define BSVM_SOP_LOADINDEXB_S		347
#define BSVM_SOP_LOADINDEXA_I		348
#define BSVM_SOP_LOADINDEXB_I		349
#define BSVM_SOP_LLOADA			350
#define BSVM_SOP_LLOADB			351
#define BSVM_SOP_UNARYOP_S		352
#define BSVM_SOP_BINARYOP_S		353
#define BSVM_SOP_ASSERT			354
#define BSVM_SOP_JOIN			355
#define BSVM_SOP_MULTIJOIN		356
#define BSVM_SOP_UNARYINTRIN_S		357
#define BSVM_SOP_BINARYINTRIN_S		358
#define BSVM_SOP_TRINARYINTRIN_S		359
#define BSVM_SOP_CONV_FN2XI		360
#define BSVM_SOP_CONV_FN2XL		361
#define BSVM_SOP_CONV_FL2XF		362
#define BSVM_SOP_CONV_FL2XD		363
#define BSVM_SOP_CONV_XI2FN		364
#define BSVM_SOP_CONV_XL2FN		365
#define BSVM_SOP_CONV_XF2FL		366
#define BSVM_SOP_CONV_XD2FL		367
#define BSVM_SOP_ADD_C			368
#define BSVM_SOP_SUB_C			369
#define BSVM_SOP_MUL_C			370
#define BSVM_SOP_DIV_C			371
#define BSVM_SOP_MOD_C			372
#define BSVM_SOP_AND_C			373
#define BSVM_SOP_OR_C			374
#define BSVM_SOP_XOR_C			375
#define BSVM_SOP_SHL_C			376
#define BSVM_SOP_SHR_C			377
#define BSVM_SOP_SHRR_C			378
#define BSVM_SOP_CMP_EQ_C		380
#define BSVM_SOP_CMP_NE_C		381
#define BSVM_SOP_CMP_LT_C		382
#define BSVM_SOP_CMP_GT_C		383
#define BSVM_SOP_CMP_LE_C		384
#define BSVM_SOP_CMP_GE_C		385
#define BSVM_SOP_JMP_EQ_C		386
#define BSVM_SOP_JMP_NE_C		387
#define BSVM_SOP_JMP_LT_C		388
#define BSVM_SOP_JMP_GT_C		389
#define BSVM_SOP_JMP_LE_C		390
#define BSVM_SOP_JMP_GE_C		391
#define BSVM_SOP_JMP_EQ_LC		392
#define BSVM_SOP_JMP_NE_LC		393
#define BSVM_SOP_JMP_LT_LC		394
#define BSVM_SOP_JMP_GT_LC		395
#define BSVM_SOP_JMP_LE_LC		396
#define BSVM_SOP_JMP_GE_LC		397
#define BSVM_SOP_PF_HINT_XUBI		400
#define BSVM_SOP_PF_HINT_XSBI		401
#define BSVM_SOP_PF_HINT_XUSI		402
#define BSVM_SOP_PF_HINT_XSSI		403
#define BSVM_SOP_PF_HINT_XUDI		404
#define BSVM_SOP_PF_HINT_XSDI		405
#define BSVM_SOP_PF_HINT_XUDL		406
#define BSVM_SOP_PF_HINT_XSDL		407
#define BSVM_SOP_PF_HINT_XUBI_IX		408
#define BSVM_SOP_PF_HINT_XSBI_IX		409
#define BSVM_SOP_PF_HINT_XUSI_IX		410
#define BSVM_SOP_PF_HINT_XSSI_IX		411
#define BSVM_SOP_PF_HINT_XUDI_IX		412
#define BSVM_SOP_PF_HINT_XSDI_IX		413
#define BSVM_SOP_PF_HINT_XUDL_IX		414
#define BSVM_SOP_PF_HINT_XSDL_IX		415
#define BSVM_SOP_PF_HINT_XII		416
#define BSVM_SOP_PF_HINT_XLL		417
#define BSVM_SOP_PF_HINT_XFF		418
#define BSVM_SOP_PF_HINT_XDD		419
#define BSVM_SOP_PF_HINT_XII_IX		420
#define BSVM_SOP_PF_HINT_XLL_IX		421
#define BSVM_SOP_PF_HINT_XFF_IX		422
#define BSVM_SOP_PF_HINT_XDD_IX		423
#define BSVM_SOP_PF_HINT_IX		425
#define BSVM_SOP_PF_HINT_FN		426
#define BSVM_SOP_PF_HINT_FL		427
#define BSVM_SOP_PF_HINT_V2F		428
#define BSVM_SOP_PF_HINT_V3F		429
#define BSVM_SOP_PF_HINT_V4F		430
#define BSVM_SOP_PF_HINT_QUAT		431
#define BSVM_SOP_PF_HINT_VOID		432
#define BSVM_SOP_PF_HINT_PVAR		433
#define BSVM_SOP_PF_HINT_RESV2		434
#define BSVM_SOP_PF_HINT_RESV3		435
#define BSVM_SOP_CONV_XI2XL		436
#define BSVM_SOP_CONV_XI2XF		437
#define BSVM_SOP_CONV_XI2XD		438
#define BSVM_SOP_CONV_XL2XI		439
#define BSVM_SOP_CONV_XL2XF		440
#define BSVM_SOP_CONV_XL2XD		441
#define BSVM_SOP_CONV_XF2XI		442
#define BSVM_SOP_CONV_XF2XL		443
#define BSVM_SOP_CONV_XF2XD		444
#define BSVM_SOP_CONV_XD2XI		445
#define BSVM_SOP_CONV_XD2XL		446
#define BSVM_SOP_CONV_XD2XF		447
#define BSVM_SOP_EXCH_YXZ		448
#define BSVM_SOP_EXCH_YZX		449
#define BSVM_SOP_EXCH_ZXY		450
#define BSVM_SOP_EXCH_ZYX		451
#define BSVM_SOP_JMP_SWEQ		456
#define BSVM_SOP_JMP_SWEQF		457
#define BSVM_SOP_JMP_SWDFL		458
#define BSVM_SOP_JMP_SWB			459
#define BSVM_SOP_JMP_U			462
#define BSVM_SOP_CMP_U			463
#define BSVM_SOP_JMP_EQ			464
#define BSVM_SOP_JMP_NE			465
#define BSVM_SOP_JMP_LT			466
#define BSVM_SOP_JMP_GT			467
#define BSVM_SOP_JMP_LE			468
#define BSVM_SOP_JMP_GE			469
#define BSVM_SOP_CMP_EQ			470
#define BSVM_SOP_CMP_NE			471
#define BSVM_SOP_CMP_LT			472
#define BSVM_SOP_CMP_GT			473
#define BSVM_SOP_CMP_LE			474
#define BSVM_SOP_CMP_GE			475
#define BSVM_SOP_JMP_EQQ			476
#define BSVM_SOP_CMP_EQQ			477
#define BSVM_SOP_JMP_NEQ			478
#define BSVM_SOP_CMP_NEQ			479
#define BSVM_SOP_EXCH_WXYZ		480
#define BSVM_SOP_EXCH_WXZY		481
#define BSVM_SOP_EXCH_WYXZ		482
#define BSVM_SOP_EXCH_WYZX		483
#define BSVM_SOP_EXCH_WZXY		484
#define BSVM_SOP_EXCH_WZYX		485
#define BSVM_SOP_EXCH_XWYZ		486
#define BSVM_SOP_EXCH_XWZY		487
#define BSVM_SOP_EXCH_XYWZ		488
#define BSVM_SOP_EXCH_XYZW		489
#define BSVM_SOP_EXCH_XZWY		490
#define BSVM_SOP_EXCH_XZYW		491
#define BSVM_SOP_EXCH_YWXZ		492
#define BSVM_SOP_EXCH_YWZX		493
#define BSVM_SOP_EXCH_YXWZ		494
#define BSVM_SOP_EXCH_YXZW		495
#define BSVM_SOP_EXCH_YZWX		496
#define BSVM_SOP_EXCH_YZXW		497
#define BSVM_SOP_EXCH_ZWXY		498
#define BSVM_SOP_EXCH_ZWYX		499
#define BSVM_SOP_EXCH_ZXWY		500
#define BSVM_SOP_EXCH_ZXYW		501
#define BSVM_SOP_EXCH_ZYWX		502
#define BSVM_SOP_EXCH_ZYXW		503
#define BSVM_SOP_SLEEP			504
#define BSVM_SOP_SLEEP_SV		505
#define BSVM_SOP_UNQUOTE			506
#define BSVM_SOP_LOCK_OBJ		507
#define BSVM_SOP_UNLOCK_OBJ		508
#define BSVM_SOP_LOADMINDEX		509
#define BSVM_SOP_STOREMINDEX		510
#define BSVM_SOP_BINDPATTERN		511
#define BSVM_SOP_BEGIN_PACKAGE		512
#define BSVM_SOP_BEGIN_STRUCT		513
#define BSVM_SOP_BEGIN_CLASS		514
#define BSVM_SOP_BEGIN_IFACE		515
#define BSVM_SOP_END_PACKAGE		516
#define BSVM_SOP_END_STRUCT		517
#define BSVM_SOP_END_CLASS		518
#define BSVM_SOP_END_IFACE		519
#define BSVM_SOP_OFFSETOF		520
#define BSVM_SOP_SIZEOF			521
#define BSVM_SOP_SIZEOF_S		522
#define BSVM_SOP_NEWARRAY_S		523
#define BSVM_SOP_NEWMARRAY_S		524
#define BSVM_SOP_CALL_THIS		525
#define BSVM_SOP_CALL_SUPER		526
#define BSVM_SOP_NEW			527
#define BSVM_SOP_PUSH_ERROR		528
#define BSVM_SOP_POP_ERROR		529
#define BSVM_SOP_INSTANCEOF		530
#define BSVM_SOP_LDJMP			531
#define BSVM_SOP_JMPIA			532
#define BSVM_SOP_EXCH_TOP		533
#define BSVM_SOP_CALL_ASYNC		534
#define BSVM_SOP_CALL_IFDEF		535
#define BSVM_SOP_CALL_IFNDEF		536
#define BSVM_SOP_BEGIN_SYNC		537
#define BSVM_SOP_END_SYNC		538
#define BSVM_SOP_BEGIN_SYNC_OBJ		539
#define BSVM_SOP_END_SYNC_OBJ		540
#define BSVM_SOP_PF_HINT_PUB		688
#define BSVM_SOP_PF_HINT_PSB		689
#define BSVM_SOP_PF_HINT_PUS		690
#define BSVM_SOP_PF_HINT_PSS		691
#define BSVM_SOP_PF_HINT_PUD		692
#define BSVM_SOP_PF_HINT_PSD		693
#define BSVM_SOP_PF_HINT_PUL		694
#define BSVM_SOP_PF_HINT_PSL		695
#define BSVM_SOP_PF_HINT_PF		696
#define BSVM_SOP_PF_HINT_PD		697
#define BSVM_SOP_PF_HINT_PV		698
#define BSVM_SOP_PF_HINT_PP		699
#define BSVM_SOP_JMP_EQ_XI		728
#define BSVM_SOP_JMP_NE_XI		729
#define BSVM_SOP_JMP_EQ_XL		730
#define BSVM_SOP_JMP_NE_XL		731
#define BSVM_SOP_JMP_EQ_XF		732
#define BSVM_SOP_JMP_NE_XF		733
#define BSVM_SOP_JMP_EQ_XD		734
#define BSVM_SOP_JMP_NE_XD		735
#define BSVM_SOP_JMP_LT_XI		736
#define BSVM_SOP_JMP_GT_XI		737
#define BSVM_SOP_JMP_LE_XI		738
#define BSVM_SOP_JMP_GE_XI		739
#define BSVM_SOP_JMP_LT_XL		740
#define BSVM_SOP_JMP_GT_XL		741
#define BSVM_SOP_JMP_LE_XL		742
#define BSVM_SOP_JMP_GE_XL		743
#define BSVM_SOP_JMP_LT_XF		744
#define BSVM_SOP_JMP_GT_XF		745
#define BSVM_SOP_JMP_LE_XF		746
#define BSVM_SOP_JMP_GE_XF		747
#define BSVM_SOP_JMP_LT_XD		748
#define BSVM_SOP_JMP_GT_XD		749
#define BSVM_SOP_JMP_LE_XD		750
#define BSVM_SOP_JMP_GE_XD		751
#define BSVM_SOP_LLOAD_XI		752
#define BSVM_SOP_LSTORE_XI		753
#define BSVM_SOP_LLOAD_XL		754
#define BSVM_SOP_LSTORE_XL		755
#define BSVM_SOP_LLOAD_XF		756
#define BSVM_SOP_LSTORE_XF		757
#define BSVM_SOP_LLOAD_XD		758
#define BSVM_SOP_LSTORE_XD		759
#define BSVM_SOP_DUP_XI			760
#define BSVM_SOP_DUP_XL			761
#define BSVM_SOP_DUP_XF			762
#define BSVM_SOP_DUP_XD			763
#define BSVM_SOP_ADD_XI			784
#define BSVM_SOP_SUB_XI			785
#define BSVM_SOP_MUL_XI			786
#define BSVM_SOP_DIV_XI			787
#define BSVM_SOP_ADD_XL			788
#define BSVM_SOP_SUB_XL			789
#define BSVM_SOP_MUL_XL			790
#define BSVM_SOP_DIV_XL			791
#define BSVM_SOP_ADD_XF			792
#define BSVM_SOP_SUB_XF			793
#define BSVM_SOP_MUL_XF			794
#define BSVM_SOP_DIV_XF			795
#define BSVM_SOP_ADD_XD			796
#define BSVM_SOP_SUB_XD			797
#define BSVM_SOP_MUL_XD			798
#define BSVM_SOP_DIV_XD			799
#define BSVM_SOP_MOD_XI			800
#define BSVM_SOP_AND_XI			801
#define BSVM_SOP_OR_XI			802
#define BSVM_SOP_XOR_XI			803
#define BSVM_SOP_MOD_XL			804
#define BSVM_SOP_AND_XL			805
#define BSVM_SOP_OR_XL			806
#define BSVM_SOP_XOR_XL			807
#define BSVM_SOP_CMP_EQ_XI		808
#define BSVM_SOP_CMP_NE_XI		809
#define BSVM_SOP_CMP_EQ_XL		810
#define BSVM_SOP_CMP_NE_XL		811
#define BSVM_SOP_CMP_EQ_XF		812
#define BSVM_SOP_CMP_NE_XF		813
#define BSVM_SOP_CMP_EQ_XD		814
#define BSVM_SOP_CMP_NE_XD		815
#define BSVM_SOP_CMP_LT_XI		816
#define BSVM_SOP_CMP_GT_XI		817
#define BSVM_SOP_CMP_LE_XI		818
#define BSVM_SOP_CMP_GE_XI		819
#define BSVM_SOP_CMP_LT_XL		820
#define BSVM_SOP_CMP_GT_XL		821
#define BSVM_SOP_CMP_LE_XL		822
#define BSVM_SOP_CMP_GE_XL		823
#define BSVM_SOP_CMP_LT_XF		824
#define BSVM_SOP_CMP_GT_XF		825
#define BSVM_SOP_CMP_LE_XF		826
#define BSVM_SOP_CMP_GE_XF		827
#define BSVM_SOP_CMP_LT_XD		828
#define BSVM_SOP_CMP_GT_XD		829
#define BSVM_SOP_CMP_LE_XD		830
#define BSVM_SOP_CMP_GE_XD		831
#define BSVM_SOP_ADD_XI_C		832
#define BSVM_SOP_SUB_XI_C		833
#define BSVM_SOP_MUL_XI_C		834
#define BSVM_SOP_DIV_XI_C		835
#define BSVM_SOP_ADD_XL_C		836
#define BSVM_SOP_SUB_XL_C		837
#define BSVM_SOP_MUL_XL_C		838
#define BSVM_SOP_DIV_XL_C		839
#define BSVM_SOP_NEG_XI			840
#define BSVM_SOP_NOT_XI			841
#define BSVM_SOP_NEG_XL			842
#define BSVM_SOP_NOT_XL			843
#define BSVM_SOP_NEG_XF			844
#define BSVM_SOP_NOT_XF			845
#define BSVM_SOP_NEG_XD			846
#define BSVM_SOP_NOT_XD			847
#define BSVM_SOP_SHL_XI			848
#define BSVM_SOP_SHR_XI			849
#define BSVM_SOP_SHL_XI_C		850
#define BSVM_SOP_SHR_XI_C		851
#define BSVM_SOP_SHL_XL			852
#define BSVM_SOP_SHR_XL			853
#define BSVM_SOP_SHL_XL_C		854
#define BSVM_SOP_SHR_XL_C		855
#define BSVM_SOP_SHRR_XI			856
#define BSVM_SOP_SHRR_XL			857
#define BSVM_SOP_SHRR_XI_C		858
#define BSVM_SOP_SHRR_XL_C		859
