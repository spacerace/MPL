/*
**  E R R O R S . H
**
**  DOS Standard Errors returned in AX.
**
**  For C Spot Run C Add-On Library, data from Norton's book.
**
**  Created: 05/05/86   Last Updated: 05/05/86
**
*/

static char *dos_error[19] = 
 {
  "",
  "Invalid function number",
  "File not found",
  "Path not found",
  "No handle available: all in use",
  "Access denied",
  "Invalid handle",
  "Memory control blocks destroyed",
  "Insuffiecient memory",
  "Invalid memory block address",
  "Invalid environment",
  "Invalid format",
  "Invalid access code",
  "Invalid data",
  "",
  "Invalid drive specification",
  "Attempt to remove current directory",
  "Not same device",
  "No more files to be found"
 };
