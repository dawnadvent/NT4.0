/* VS_W6.C 13/04/95 16.01.02 */
VW_ENTRYSC SHORT VW_ENTRYMOD VwStreamOpenFunc (SOFILE fp, SHORT wFileId, BYTE
	 VWPTR *pFileName, SOFILTERINFO VWPTR *pFilterInfo, HPROC hProc);
VW_LOCALSC WORD VW_LOCALMOD AllocateMemory (HANDLE VWPTR *h, LPBYTE VWPTR *lp,
	 WORD Size, WORD *Ok, HPROC hProc);
VW_LOCALSC TABS VWPTR *VW_LOCALMOD TabstopHandler (WORD nNewTabs, WORD nOldTabs
	, TABS VWPTR *Tabs, HPROC hProc);
VW_LOCALSC VOID VW_LOCALMOD stsh_parser (HPROC hProc);
VW_ENTRYSC SHORT VW_ENTRYMOD VwStreamSectionFunc (SOFILE fp, HPROC hProc);
VW_LOCALSC VOID VW_LOCALMOD load_fkp (REGISTER FKPWIN *fkp, DWORD fc, BTE VWPTR
	 *bte, DWORD fcPlcfbte, HPROC hProc);
VW_LOCALSC DWORD VW_LOCALMOD fGetLong (HPROC hProc);
VW_LOCALSC DWORD VW_LOCALMOD fGetLong2 (LPBYTE VWPTR *ptbyte, HPROC hProc);
VW_LOCALSC WORD VW_LOCALMOD mGetWord (LPBYTE VWPTR *ptbyte, HPROC hProc);
VW_LOCALSC WORD VW_LOCALMOD mGetByte (LPBYTE VWPTR *ptbyte, HPROC hProc);
VW_LOCALSC DWORD VW_LOCALMOD mGetLong (LPBYTE VWPTR *ptbyte, HPROC hProc);
VW_LOCALSC WORD VW_LOCALMOD fGetByte (LPBYTE VWPTR *ptbyte, HPROC hProc);
VW_LOCALSC WORD VW_LOCALMOD fGetWord (HPROC hProc);
VW_LOCALSC WORD VW_LOCALMOD fGetWord2 (LPBYTE VWPTR *ptbyte, HPROC hProc);
VW_LOCALSC VOID VW_LOCALMOD piece_handler (HPROC hProc);
VW_LOCALSC VOID VW_LOCALMOD piece_reader (DWORD length, HPROC hProc);
VW_LOCALSC VOID VW_LOCALMOD pap_copy (PAP VWPTR *papt, PAP VWPTR *papf, HPROC
	 hProc);
VW_LOCALSC VOID VW_LOCALMOD LoadistdBuffer (DWORD Offset, HPROC hProc);
VW_LOCALSC WORD VW_LOCALMOD slot_builder (WORD istd, HPROC hProc);
VW_LOCALSC VOID VW_LOCALMOD style_builder (WORD istd, HPROC hProc);
VW_LOCALSC WORD VW_LOCALMOD style_handler (WORD istd, HPROC hProc);
VW_LOCALSC BYTE VW_LOCALMOD sprmCF (WORD fAttrStyle, WORD Val, HPROC hProc);
VW_LOCALSC VOID VW_LOCALMOD spermatic (SHORT length_papx, BYTE VWPTR *storage,
	 BYTE disk, BYTE istctoread, HPROC hProc);
VW_LOCALSC VOID VW_LOCALMOD chp_init (HPROC hProc);
VW_LOCALSC VOID VW_LOCALMOD SetSymbolAttributes (REGISTER CHP VWPTR *chp1,
	 REGISTER CHP VWPTR *chp2, HPROC hProc);
VW_LOCALSC VOID VW_LOCALMOD pap_init (HPROC hProc);
VW_LOCALSC VOID VW_LOCALMOD sep_processor (HPROC hProc);
VW_LOCALSC VOID VW_LOCALMOD SetParaAttributes (REGISTER PAP VWPTR *pap1,
	 REGISTER PAP VWPTR *pap2, HPROC hProc);
VW_LOCALSC VOID VW_LOCALMOD prm_processor (WORD prm, HPROC hProc);
VW_LOCALSC VOID VW_LOCALMOD modifier_processor (WORD ProcessFlag, HPROC hProc);
VW_LOCALSC VOID VW_LOCALMOD PicReader (HPROC hProc);
VW_LOCALSC VOID VW_LOCALMOD PicHandler (WORD PicType, HPROC hProc);
VW_LOCALSC WORD VW_LOCALMOD DefineShadow (WORD Shade, HPROC hProc);
VW_LOCALSC VOID VW_LOCALMOD DefineBorders (WORD brc, SOBORDER VWPTR *soBorder,
	 HPROC hProc);
VW_LOCALSC VOID VW_LOCALMOD SpecialSymbol (WORD Symbol, WORD CheckSpecial,
	 HPROC hProc);
VW_ENTRYSC SHORT VW_ENTRYMOD VwStreamReadFunc (SOFILE fp, HPROC hProc);
VW_LOCALSC WORD VW_LOCALMOD prop_finder (BTE VWPTR *bte, DWORD fcPlcfbte, DWORD
	 fc, HPROC hProc);
VW_LOCALSC VOID VW_LOCALMOD character_prop_finder (HPROC hProc);
VW_LOCALSC VOID VW_LOCALMOD paragraph_end_finder (HPROC hProc);
VW_LOCALSC VOID VW_LOCALMOD FreeMemory (HPROC hProc);
VW_ENTRYSC VOID VW_ENTRYMOD VwStreamCloseFunc (SOFILE hFile, HPROC hProc);
VW_ENTRYSC SHORT VW_ENTRYMOD VwStreamTellFunc (SOFILE fp, HPROC hProc);
VW_ENTRYSC SHORT VW_ENTRYMOD VwStreamSeekFunc (SOFILE hFile, HPROC hProc);
