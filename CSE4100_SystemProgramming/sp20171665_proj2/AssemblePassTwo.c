#include "20171665.h"

/*------------------------------------------------------------------------------------*/
/*함수 : AssemblePassTwo(char *AssembleFileName)*/
/*목적 : Pass 2 시행
/*리턴값 : 0은 성공, -1은 실패*/
/*------------------------------------------------------------------------------------*/
int AssemblePassTwo(char *AssembleFileName) {
	FILE *ImmediateFilePointer = NULL, *ListFilePointer = NULL, *ObjectFilePointer = NULL;

	char *FileName = NULL, *ImmediateFileName = NULL, *ListFileName = NULL, *ObjectFileName = NULL;
	int FileNameLength = 0, ErrorFlag = 0;

	if (strlen(AssembleFileName) <= 4) {
		return -1;
	}
	FileName = (char*)malloc((FileNameLength = sizeof(char) * (int)strlen(AssembleFileName) + 1) - 4);

	strncpy(FileName, AssembleFileName, FileNameLength - 5);
	FileName[FileNameLength - 5] = '\0';

	ImmediateFileName = (char*)malloc(FileNameLength);
	snprintf(ImmediateFileName, FileNameLength, "%s.imm", FileName);
	ImmediateFileName[FileNameLength - 1] = '\0';

	if ((ImmediateFilePointer = fopen(ImmediateFileName, "r")) == NULL) {
		printf("An error occurs while opening the file you want to assemble.\n");
		if (FileName != NULL) {
			free(FileName);
		}
		if (ImmediateFileName != NULL) {
			free(ImmediateFileName);
		}
		return -1;
	}

	ListFileName = (char*)malloc(FileNameLength);
	ObjectFileName = (char*)malloc(FileNameLength);

	snprintf(ListFileName, FileNameLength, "%s.lst", FileName);
	ListFileName[FileNameLength - 1] = '\0';

	snprintf(ObjectFileName, FileNameLength, "%s.obj", FileName);
	ObjectFileName[FileNameLength - 1] = '\0';

	if ((ListFilePointer = fopen(ListFileName, "w")) == NULL) {
		printf("An error occurs while writing the list file.\n");
		return -1;
	}

	if ((ObjectFilePointer = fopen(ObjectFileName, "w")) == NULL) {
		printf("An error occurs while writing the list file.\n");
		return -1;
	}

	int HexAddressCounter = 0, LabelCounter = 0, OpcodeMnemonicCounter = 0, OperandCounter = 0, LineNumber = 0, TempBuffer = 0;
	int AddLocationCounter = 0, BaseRegister = 0, ProgramCounter = 0, RelocationCounter = 0, RealOperand = 0;
	int Flag = 0, NIFlag = 0, BaseFlag = 0, IndexFlag = 0, MemoryFlag = 0, ReserveFlag = 0, CommentFlag = 0;
	char BufferChar = EOF, *HexAddress = NULL, *Label = NULL, *OpcodeMnemonic = NULL, *Operand = NULL, *TempOperand = NULL;
	char HexCode[12] = { '\0' }, HexCodeOpcodeAndNI[3] = { '\0' }, HexCodeXBPE[2] = { '\0' }, HexCodeOperand[6] = { '\0' };
	char FirstRegister[3] = { '\0' }, SecondRegister[3] = { '\0' }, MemoryContent[200] = { '\0' }, ProgramName[7] = { '\0' };
	char TextRecordStartAddress[7] = { '\0' }, TextRecordObjectCode[61] = { '\0' }, ModifiedAddress[10000][7] = { '\0' };
	int TextRecordCounter = 0;

	while (1) {
		HexAddress = (char*)realloc(HexAddress, sizeof(char) * (HexAddressCounter + 1));
		Label = (char*)realloc(Label, sizeof(char) * (LabelCounter + 1));
		OpcodeMnemonic = (char*)realloc(OpcodeMnemonic, sizeof(char) * (OpcodeMnemonicCounter + 1));
		Operand = (char*)realloc(Operand, sizeof(char) * (OperandCounter + 1));
		TempOperand = (char*)realloc(TempOperand, sizeof(char) * (OperandCounter + 1));
		LineNumber += 5;

		while ((BufferChar = fgetc(ImmediateFilePointer)) != '\n') {
			// Line의 첫 character로 '.'를 읽으면
			if ((LabelCounter == 0) && (BufferChar == '.')) {
				fprintf(ListFilePointer, "\t%c", BufferChar);
				while ((BufferChar = fgetc(ImmediateFilePointer)) != '\n') {
					fprintf(ListFilePointer, "%c", BufferChar);
				}
				fprintf(ListFilePointer, "%c", BufferChar);
				CommentFlag = 1;
				break;
			}
			else if (BufferChar == '\t') {
				// 첫 character로 공백이 들어온 경우
				if (Flag == 0) {
					Flag = 1;
				}
				// Label과 Opcode를 모두 읽고 공백이 들어온 경우
				else if (Flag == 1) {
					Flag = 2;
				}
				else if (OpcodeMnemonicCounter && (Flag == 2)) {
					Flag = 3;
				}
				continue;
			}
			else if (BufferChar == EOF) {
				break;
			}
			else {
				switch (Flag) {
				case 0:
					HexAddress = (char*)realloc(HexAddress, sizeof(char) * (HexAddressCounter + 2));
					HexAddress[HexAddressCounter++] = BufferChar;
					break;
				case 1:
					Label = (char*)realloc(Label, sizeof(char) * (LabelCounter + 2));
					Label[LabelCounter++] = BufferChar;
					break;
				case 2:
					OpcodeMnemonic = (char*)realloc(OpcodeMnemonic, sizeof(char) * (OpcodeMnemonicCounter + 2));
					OpcodeMnemonic[OpcodeMnemonicCounter++] = BufferChar;
					break;
				case 3:
					Operand = (char*)realloc(Operand, sizeof(char) * (OperandCounter + 2));
					Operand[OperandCounter++] = BufferChar;
					break;
				}
			}
		}

		if (CommentFlag) {
			CommentFlag = 0;
			continue;
		}

		HexAddress[HexAddressCounter] = Label[LabelCounter] = OpcodeMnemonic[OpcodeMnemonicCounter] = Operand[OperandCounter] = '\0';
		TempOperand = (char*)realloc(TempOperand, sizeof(char) * OperandCounter + 1);
		//printf("Label: [%s] Opcode: [%s] Operand: [%s]\n", Label, OpcodeMnemonic, Operand);

		RemoveBlank(&HexAddress);
		RemoveBlank(&Label);
		RemoveBlank(&OpcodeMnemonic);
		RemoveBlank(&Operand);

		if (OpcodeMnemonic != NULL) {
			if (strcmp(OpcodeMnemonic, "START") == 0) {
				if (Label != NULL) {
					strncpy(ProgramName, Label, sizeof(ProgramName) - 1);
					ProgramName[6] = '\0';
				}
				fprintf(ObjectFilePointer, "H%-6s%06X%06X\n", ProgramName, StartAddress & 0XFFFFFF, ProgramLength & 0XFFFFFF);
			}

			HASH *FoundHash = NULL;
			switch (SearchOpcode(OpcodeMnemonic, &FoundHash)) {
				// Opcode Table에서 Opcode를 찾으면
			case 0:
				if (strcmp(FoundHash->Format, "1") == 0) {
					AddLocationCounter = 1;
					TempBuffer = HexStringToDec(FoundHash->Number, Integer);
					snprintf(HexCodeOpcodeAndNI, sizeof(char) * 3, "%02X", TempBuffer & 0xFF);
					sprintf(HexCode, "%s", HexCodeOpcodeAndNI);
				}
				else if (strcmp(FoundHash->Format, "2") == 0) {
					AddLocationCounter = 2;
					// SHIFTL, SHIFTR Opcode가 아닐 때
					if (Operand != NULL) {
						if ((strcmp(OpcodeMnemonic, "SHIFTL") != 0) && (strcmp(OpcodeMnemonic, "SHIFTR") != 0) && (strcmp(OpcodeMnemonic, "SVC") != 0)) {
							int RegisterFlag = 0, FirstRegisterValue = 0, SecondRegisterValue = 0;

							TempBuffer = HexStringToDec(FoundHash->Number, Integer);
							snprintf(HexCodeOpcodeAndNI, sizeof(char) * 3, "%02X", TempBuffer & 0xFF);

							for (int i = 0; i < OperandCounter; i++) {
								if (Operand[i] == ',') {
									strncpy(FirstRegister, Operand, sizeof(char) * i);
									strncpy(SecondRegister, Operand + i + 1, sizeof(SecondRegister) - 1);
									RegisterFlag = 1;
									break;
								}
							}
							if (!RegisterFlag) {
								if (strcmp(OpcodeMnemonic, "CLEAR") == 0 || strcmp(OpcodeMnemonic, "TIXR") == 0) {
									if ((FirstRegisterValue = FindRegisterNumber(Operand)) != -1) {
										snprintf(FirstRegister, sizeof(char) * 3, "%01X", FirstRegisterValue & 0xF);
									}
									else {
										ErrorFlag = 1;
										printf("[Line %d(%dth line)] Please write one when you write 'CLEAR' and 'TIXR' operand.\n", LineNumber, LineNumber / 5);
										break;
									}
									sprintf(HexCode, "%s%s0", HexCodeOpcodeAndNI, FirstRegister);
								}
								else {
									ErrorFlag = 1;
									printf("[Line %d(%dth line)] Please write two registers except for 'CLEAR' and 'TIXR' operand.\n", LineNumber, LineNumber / 5);
									break;
								}
							}
							else {
								if (strcmp(OpcodeMnemonic, "CLEAR") != 0 && strcmp(OpcodeMnemonic, "TIXR") != 0) {
									if ((FirstRegisterValue = FindRegisterNumber(FirstRegister)) != -1 && (SecondRegisterValue = FindRegisterNumber(SecondRegister)) != -1) {
										snprintf(FirstRegister, sizeof(char) * 3, "%01X", FirstRegisterValue & 0xF);
										snprintf(SecondRegister, sizeof(char) * 3, "%01X", SecondRegisterValue & 0xF);
									}
									else {
										ErrorFlag = 1;
										printf("[Line %d(%dth line)] Please write two registers except for 'CLEAR' and 'TIXR' operand.\n", LineNumber, LineNumber / 5);
										break;
									}
									sprintf(HexCode, "%s%s%s", HexCodeOpcodeAndNI, FirstRegister, SecondRegister);
								}
								else {
									ErrorFlag = 1;
									printf("[Line %d(%dth line)] Please write one register when you write 'CLEAR' and 'TIXR' operand.\n", LineNumber, LineNumber / 5);
									break;
								}
							}
						}

						// SVC일 때
						else if (strcmp(OpcodeMnemonic, "SVC") == 0) {
							int Nbit = 0;
							TempBuffer = HexStringToDec(FoundHash->Number, Integer);
							snprintf(HexCodeOpcodeAndNI, sizeof(char) * 3, "%02X", TempBuffer & 0xFF);
							if (Nbit = DecStringToDec(Operand, Integer)) {
								sprintf(HexCode, "%s%01X0", HexCodeOpcodeAndNI, Nbit & 0xF);
							}
							else {
								ErrorFlag = 1;
								printf("[Line %d(%dth line)] Please check the format of N bits.\n", LineNumber, LineNumber / 5);
								break;
							}
						}
						// SHIFTL 또는 SHIFTR Opcode일 때
						else {
							int RegisterValue = 0, Nbit = 0;

							TempBuffer = HexStringToDec(FoundHash->Number, Integer);
							snprintf(HexCodeOpcodeAndNI, sizeof(char) * 3, "%02X", TempBuffer & 0xFF);
							Nbit = DecStringToDec(Operand, Integer);

							for (int i = 0; i < OperandCounter; i++) {
								if (Operand[i] == ',') {
									strncpy(FirstRegister, Operand, sizeof(char) * i);
									strncpy(SecondRegister, Operand + i + 1, sizeof(SecondRegister) - 1);
									break;
								}
							}
							if ((RegisterValue = FindRegisterNumber(FirstRegister)) != -1 && (Nbit = DecStringToDec(SecondRegister, Integer) - 1) >= 0) {
								snprintf(FirstRegister, sizeof(char) * 3, "%01X", RegisterValue & 0xF);
								snprintf(SecondRegister, sizeof(char) * 3, "%01X", Nbit & 0xF);
							}
							else {
								ErrorFlag = 1;
								printf("[Line %d(%dth line)] Please check the line. The valid format is SHIFTL r, n (or SHIFTR r, n).\n", LineNumber, LineNumber / 5);
								break;
							}
							sprintf(HexCode, "%s%s%s", HexCodeOpcodeAndNI, FirstRegister, SecondRegister);
						}
					}
					else {
						TempBuffer = HexStringToDec(FoundHash->Number, Integer);
						snprintf(HexCodeOpcodeAndNI, sizeof(char) * 3, "%02X", TempBuffer & 0xFF);
						sprintf(HexCode, "%s00", HexCodeOpcodeAndNI);
					}
				}
				else if (strcmp(FoundHash->Format, "3/4") == 0) {
					AddLocationCounter = 3;
					// Format 3일 때
					if (OpcodeMnemonic[0] != '+') {
						SYMBOL *TempSymbol = NULL;
						char *TempOperandPointer = NULL;
						TempBuffer = HexStringToDec(FoundHash->Number, Integer);
						if (Operand != NULL) {
							for (int i = 0; i < OperandCounter; i++) {
								if (i == OperandCounter - 2) {
									if (Operand[i] == ',' && Operand[i + 1] == 'X') {
										strncpy(TempOperand, Operand, sizeof(char) * i);
										TempOperand[i] = '\0';
										IndexFlag = 8;
									}
								}
							}

							if (!IndexFlag) {
								strncpy(TempOperand, Operand, sizeof(char) * OperandCounter + 1);
								TempOperand[OperandCounter] = '\0';
							}

							if (Operand[0] == '@') {
								TempBuffer += 2;
								NIFlag = IndirectAddressing;
							}
							else if (Operand[0] == '#') {
								TempBuffer += 1;
								NIFlag = ImmediateAddressing;
							}
							else {
								TempBuffer += 3;
							}
							snprintf(HexCodeOpcodeAndNI, sizeof(char) * 3, "%02X", TempBuffer & 0xFF);
							ProgramCounter = HexStringToDec(HexAddress, ForProgramCounter) + AddLocationCounter;

							if (NIFlag) {
								if (IndexFlag) {
									// Index Addressing은 Immediate 또는 Indirect Addressing과 함께 쓰일 수 없으므로 (출처: 번역본 교재 1.3장 13페이지)
									ErrorFlag = 1;
									printf("[Line %d(%dth line)] An index addressing cannot use with immediate or indirect addressing.\n", LineNumber, LineNumber / 5);
									break;
								}
								TempOperandPointer = &(TempOperand[1]);
							}
							else {
								TempOperandPointer = TempOperand;
							}

							if (SearchSymbolTable(TempOperandPointer, &TempSymbol) == 0) {
								RealOperand = TempSymbol->LabelAddress - ProgramCounter;
								if (RealOperand >= -2048 && RealOperand <= 2047) {
									snprintf(HexCodeXBPE, sizeof(char) * 2, "%01X", (TempBuffer = 2 + IndexFlag) & 0xF);
								}
								else {
									if (BaseFlag) {
										if ((RealOperand = TempSymbol->LabelAddress - BaseRegister) >= 0 && RealOperand <= 4095) {
											//printf("TA: %d / BaseRegister: %d / Displacement: %d\n", TempSymbol->LabelAddress, BaseRegister, RealOperand);
											snprintf(HexCodeXBPE, sizeof(char) * 2, "%01X", (TempBuffer = 4 + IndexFlag) & 0xF);
										}
										else {
											ErrorFlag = 1;
											printf("[Line %d(%dth line)] The relative addresssing is not valid because of an excess of the range. ", LineNumber, LineNumber / 5);
											printf("You can use an immediate addressing by writing '+' at the front of the opcode.\n");
											break;
										}
									}
									else {
										ErrorFlag = 1;
										printf("[Line %d(%dth line)] Please write the 'BASE' directive forward.\n", LineNumber, LineNumber / 5);
										break;
									}
								}
							}
							else if (NIFlag == ImmediateAddressing) {
								RealOperand = DecStringToDec(TempOperandPointer, ImmediateAddressing);
								snprintf(HexCodeXBPE, sizeof(char) * 2, "%01X", (TempBuffer = 0) & 0xF);
							}
							else {
								ErrorFlag = 1;
								printf("[Line %d(%dth line)] Please check the line. You should write '#' in the front of the instruction when you want to use an immediate addressing. ", LineNumber, LineNumber / 5);
								printf("Otherwise, you should write the valid symbol.\n");
								break;
							}

							snprintf(HexCodeOperand, sizeof(char) * 4, "%03X", RealOperand & 0xFFF);
							sprintf(HexCode, "%s%s%s", HexCodeOpcodeAndNI, HexCodeXBPE, HexCodeOperand);
						}
						else {
							TempBuffer = HexStringToDec(FoundHash->Number, Integer);
							snprintf(HexCodeOpcodeAndNI, sizeof(char) * 3, "%02X", (TempBuffer += 3) & 0xFF);
							sprintf(HexCode, "%s0000", HexCodeOpcodeAndNI);
						}
					}
					// Format 4일 때
					else {
						AddLocationCounter = 4;
						SYMBOL *TempSymbol = NULL;
						char *TempOperandPointer = NULL;
						TempBuffer = HexStringToDec(FoundHash->Number, Integer);
						if (Operand != NULL) {
							for (int i = 0; i < OperandCounter; i++) {
								if (i == OperandCounter - 2) {
									if (Operand[i] == ',' && Operand[i + 1] == 'X') {
										strncpy(TempOperand, Operand, sizeof(char) * i);
										TempOperand[i] = '\0';
										IndexFlag = 8;
									}
								}
							}

							if (!IndexFlag) {
								strncpy(TempOperand, Operand, sizeof(char) * OperandCounter + 1);
								TempOperand[OperandCounter] = '\0';
							}

							if (Operand[0] == '@') {
								TempBuffer += 2;
								NIFlag = IndirectAddressing;
							}
							if (Operand[0] == '#') {
								TempBuffer += 1;
								NIFlag = ImmediateAddressing;
							}
							else {
								TempBuffer += 3;
							}
							snprintf(HexCodeOpcodeAndNI, sizeof(char) * 3, "%02X", TempBuffer & 0xFF);

							if (NIFlag) {
								if (IndexFlag) {
									// Index Addressing은 Immediate 또는 Indirect Addressing과 함께 쓰일 수 없으므로 (출처: 번역본 교재 1.3장 13페이지)
									ErrorFlag = 1;
									printf("[Line %d(%dth line)] An index Addressing cannot use with immediate or indirect addressing.\n", LineNumber, LineNumber / 5);
									break;
								}
								TempOperandPointer = &(TempOperand[1]);
							}
							else {
								TempOperandPointer = TempOperand;
							}


							if (SearchSymbolTable(TempOperandPointer, &TempSymbol) == 0) {
								RealOperand = TempSymbol->LabelAddress;
								snprintf(HexCodeXBPE, sizeof(char) * 2, "%01X", (TempBuffer = 1 + IndexFlag) & 0xF);
								sprintf(ModifiedAddress[RelocationCounter++], "%06X", (HexStringToDec(HexAddress, ForProgramCounter) - StartAddress + 1) & 0xFFFFFF);
							}
							else if (NIFlag == ImmediateAddressing) {
								RealOperand = DecStringToDec(TempOperandPointer, ImmediateAddressing);
								snprintf(HexCodeXBPE, sizeof(char) * 2, "%01X", (TempBuffer = 1) & 0xF);
							}
							else {
								ErrorFlag = 1;
								printf("[Line %d(%dth line)] Please check the line. You should write '#' in the front of the instruction when you want to use an immediate addressing. ", LineNumber, LineNumber / 5);
								printf("Otherwise, you should write the valid symbol.\n");
								break;
							}
							snprintf(HexCodeOperand, sizeof(char) * 6, "%05X", RealOperand & 0xFFFFF);

							sprintf(HexCode, "%s%s%s", HexCodeOpcodeAndNI, HexCodeXBPE, HexCodeOperand);
						}
						else {
							TempBuffer = HexStringToDec(FoundHash->Number, Integer);
							snprintf(HexCodeOpcodeAndNI, sizeof(char) * 3, "%02X", (TempBuffer += 3) & 0xFF);
							snprintf(HexCodeXBPE, sizeof(char) * 2, "%01X", (TempBuffer = 1) & 0xF);
							snprintf(HexCodeOperand, sizeof(char) * 6, "00000");
							sprintf(HexCode, "%s%s%s", HexCodeOpcodeAndNI, HexCodeXBPE, HexCodeOperand);
						}
					}
				}
				else {

				}
				break;
				// Opcode Table에서 Opcode를 찾지 못하면
			case 1:
				if (Operand != NULL) {
					if (strcmp(OpcodeMnemonic, "WORD") == 0) {
						MemoryFlag = Word;
						sprintf(MemoryContent, "%06X", DecStringToDec(Operand, Word) & 0xFFFFFF);
						AddLocationCounter = 3;
					}
					else if (strcmp(OpcodeMnemonic, "RESW") == 0) {
						ReserveFlag = 1;
					}
					else if (strcmp(OpcodeMnemonic, "RESB") == 0) {
						ReserveFlag = 1;
					}
					else if (strcmp(OpcodeMnemonic, "BYTE") == 0) {
						if (Operand[1] == '\'') {
							int DigitNumber = 0;
							if (Operand[0] == 'X') {
								MemoryFlag = Hexadecimal;
								for (int i = 2; Operand[i] != '\''; i++) {
									DigitNumber++;
									MemoryContent[i - 2] = Operand[i];
								}
								AddLocationCounter = (DigitNumber + 1) / 2;
							}
							else if (Operand[0] == 'C') {
								MemoryFlag = Character;
								for (int i = 2; Operand[i] != '\''; i++) {
									DigitNumber++;
									sprintf(MemoryContent, "%s%02X", MemoryContent, Operand[i] & 0xFF);
								}
								AddLocationCounter = DigitNumber;
							}

						}
					}
					else if (strcmp(OpcodeMnemonic, "BASE") == 0) {
						SYMBOL *BaseSymbol = NULL;
						if (SearchSymbolTable(Operand, &BaseSymbol) == 0) {
							BaseRegister = BaseSymbol->LabelAddress;
							BaseFlag = 1;
						}
					}
					else if (strcmp(OpcodeMnemonic, "NOBASE") == 0) {
						BaseRegister = 0;
						BaseFlag = 0;
					}
				}
				break;
			default:
				break;
			}
			if (ErrorFlag == 1) {
				break;
			}
			if (strcmp(OpcodeMnemonic, "END") == 0) {
				if (TextRecordCounter) {
					fprintf(ObjectFilePointer, "T%-6s%02X%-30s\n", TextRecordStartAddress, TextRecordCounter & 0xFF, TextRecordObjectCode);
				}
				for (int i = 0; i < RelocationCounter; i++) {
					fprintf(ObjectFilePointer, "M%s05\n", ModifiedAddress[i]);
				}
				if (Operand != NULL) {
					SYMBOL *StartAddressSymbol = NULL;
					if (SearchSymbolTable(Operand, &StartAddressSymbol) == 0) {
						fprintf(ObjectFilePointer, "E%06X\n", StartAddressSymbol->LabelAddress & 0xFFFFFF);
					}
					else {
						ErrorFlag = 1;
						printf("[Line %d(%dth line)] The label of starting address isn't found.\n", LineNumber, LineNumber / 5);
						break;
					}
				}
				else {
					fprintf(ObjectFilePointer, "E%06X\n", StartAddress & 0xFFFFFF);
				}

			}

		}
		if (ErrorFlag == 1) {
			break;
		}
		if (Flag) {
			// LOCCTR
			if (HexAddress != NULL) {
				fprintf(ListFilePointer, "%s\t", HexAddress);
			}
			else {
				fprintf(ListFilePointer, "\t");
			}

			// SYMBOL
			if (Label != NULL) {
				fprintf(ListFilePointer, "%s\t", Label);
			}
			else {
				fprintf(ListFilePointer, "\t");
			}


			// Opcode Mnemonic
			if (OpcodeMnemonic != NULL) {
				// Object Code를 작성했을 때 Text Record의 길이가 30 byte를 초과하거나 예약어에 의해서 로더에 업로드될 데이터가 있을 때 
				if ((TextRecordCounter + AddLocationCounter > 30) || (ReserveFlag && TextRecordCounter)) {
					fprintf(ObjectFilePointer, "T%-6s%02X%-30s\n", TextRecordStartAddress, TextRecordCounter & 0xFF, TextRecordObjectCode);
					memset(TextRecordStartAddress, '\0', sizeof(TextRecordStartAddress));
					memset(TextRecordObjectCode, '\0', sizeof(TextRecordObjectCode));
					TextRecordCounter = 0;
				}
				fprintf(ListFilePointer, "%s\t", OpcodeMnemonic);
			}
			else {
				fprintf(ListFilePointer, "\t");
			}

			// Operand
			if (Operand != NULL) {
				fprintf(ListFilePointer, "%s\t", Operand);
			}
			else {
				fprintf(ListFilePointer, "\t");
			}

			// ObjectCode
			if (HexCode[0] != '\0' || MemoryFlag) {
				if (!TextRecordCounter && !ReserveFlag) {
					sprintf(TextRecordStartAddress, "%06X", HexStringToDec(HexAddress, ForProgramCounter) & 0xFFFFFF);
				}
				if (HexCode[0] != '\0' && !MemoryFlag) {
					fprintf(ListFilePointer, "%s\t", HexCode);
					sprintf(TextRecordObjectCode, "%s%s", TextRecordObjectCode, HexCode);
				}
				else if (MemoryFlag) {
					fprintf(ListFilePointer, "%s\t", MemoryContent);
					sprintf(TextRecordObjectCode, "%s%s", TextRecordObjectCode, MemoryContent);
				}
				TextRecordCounter += AddLocationCounter;
			}
			else {
				fprintf(ListFilePointer, "\t");
			}
			fprintf(ListFilePointer, "\n");
		}

		HexAddressCounter = LabelCounter = OpcodeMnemonicCounter = OperandCounter = AddLocationCounter = 0;
		ReserveFlag = IndexFlag = NIFlag = MemoryFlag = Flag = 0;

		memset(HexCode, '\0', sizeof(HexCode));
		memset(HexCodeOpcodeAndNI, '\0', sizeof(HexCodeOpcodeAndNI));
		memset(HexCodeXBPE, '\0', sizeof(HexCodeXBPE));
		memset(HexCodeOperand, '\0', sizeof(HexCodeOperand));
		memset(FirstRegister, '\0', sizeof(FirstRegister));
		memset(SecondRegister, '\0', sizeof(SecondRegister));
		memset(MemoryContent, '\0', sizeof(MemoryContent));

		if (BufferChar == EOF) {
			break;
		}
	}

	fclose(ImmediateFilePointer);
	fclose(ListFilePointer);
	fclose(ObjectFilePointer);
	remove(ImmediateFileName);

	if (ErrorFlag == 1) {
		DeleteSymbolTable();
		remove(ListFileName);
		remove(ObjectFileName);
	}
	else {
		printf("output file: [%s], [%s]\n", ListFileName, ObjectFileName);
	}

	if (HexAddress != NULL) {
		free(HexAddress);
	}
	if (Label != NULL) {
		free(Label);
	}
	if (OpcodeMnemonic != NULL) {
		free(OpcodeMnemonic);
	}
	if (Operand != NULL) {
		free(Operand);
	}
	if (TempOperand != NULL) {
		free(TempOperand);
	}
	if (FileName != NULL) {
		free(FileName);
	}
	if (ImmediateFileName != NULL) {
		free(ImmediateFileName);
	}
	if (ListFileName != NULL) {
		free(ListFileName);
	}
	if (ObjectFileName != NULL) {
		free(ObjectFileName);
	}

	return 0;
}