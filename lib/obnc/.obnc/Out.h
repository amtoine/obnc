/*GENERATED BY OBNC 0.9.1*/

#ifndef Out_h
#define Out_h

#include <obnc/OBNC.h>

#define Out_Open_ obnc_Out_Open_
void Out_Open_(void);

#define Out_Char_ obnc_Out_Char_
void Out_Char_(char ch_);

#define Out_String_ obnc_Out_String_
void Out_String_(const char s_[], int s_len);

#define Out_Int_ obnc_Out_Int_
void Out_Int_(int i_, int n_);

#define Out_Real_ obnc_Out_Real_
void Out_Real_(double x_, int n_);

#define Out_Ln_ obnc_Out_Ln_
void Out_Ln_(void);

#define Out_Init obnc_Out_Init
void Out_Init(void);

#endif
