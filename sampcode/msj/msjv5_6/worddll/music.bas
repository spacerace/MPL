REM MUSIC -- a WordBasic macro 
Declare Sub SetVoiceQueueSize Lib "sound"(voice As Integer, n As Integer) 
Declare Sub OpenSound Lib "sound"(foo As Integer) 
Declare Sub CloseSound Lib "sound"(foo As Integer) 
Declare Sub StartSound Lib "sound"(foo As Integer) 
Declare Sub StopSound Lib "sound"(foo As Integer) 
Declare Sub SetVoiceNote Lib "sound"(nVoice As Integer, nValue As Integer, 
nlength As Integer, nCdots As Integer) 
Declare Sub SetVoiceSound Lib "sound"(nVoice As Integer, nFreq As Integer, 
nDuration As Integer) 
Declare Sub SetVoiceAccent Lib "sound"(nVoice As Integer, nTempo As Integer, 
nVolume As Integer, nMode As Integer, nPitch As Integer) 
Sub MAIN 
Notes$ = "G1a2bc3d4ef5g6A7BC8D9EF0" 
Song$ = Selection$() 
voice = 1 
OpenSound(1) 
StopSound(1) 
SetVoiceQueueSize(voice, 10 +(Len(song$) * 6)) 
Duration = 8 
For i = 1 To Len(song$) 
   If Mid$(Song$, i, 1) = "+" Then 
        Duration = 4 
   ElseIf(Mid$(song$, i, 1)) = "-" Then 
         Duration = 8 
   ElseIf(Mid$(song$, i, 1)) = "/" Then 
         duration = 16 
   ElseIf InStr(Notes$, Mid$(Song$, i, 1)) <> 0 Then 
         SetVoiceNote(voice, InStr(Notes$, Mid$(Song$, i, 1)) + 19, Duration, 0) 
   End If 
Next i 
StartSound(1) 
REM song1 -- cdeccdecef+g-ef+g/gAgf-ec/gAgf-ec-cG+c-cG+c 
REM song 2 - +cc-cd+e-edef+gg/CCCgggeee+c-gfed+c 
REM song 3 - eefggfedccde+e/d+d-eefggfedccde+d/c+c-ddecd/ef-ecd/ef-
edcd+G-eefggfedccde+d/c+c 
End Sub 
