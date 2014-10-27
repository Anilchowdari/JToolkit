/*
    Wn: A Server for the HTTP
    File: wn/content.h
    Version 1.17.8
    
    Copyright (C) 1995  <by John Franks>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/


#define MAXMIME		(150)

#ifdef WNDEX
static char * 
#else
WN_CONST
static char * 
WN_CONST
#endif
mimelist[MAXMIME][2] = {
		{ "html", "text/html"},
		{ "htm", "text/html"},
		{ "gif", "image/gif"},
		{ "txt", "text/plain"},
		{ "ps", "application/postscript"},
		{ "jpeg", "image/jpeg"},
		{ "jpg", "image/jpeg"},
		{ "jpe", "image/jpeg"},
		{ "tex", "text/plain"},
		{ "csm", "application/cu-seeme"},
		{ "cu", "application/cu-seeme"},
		{ "tsp", "application/dsptype"},
		{ "xls", "application/excel"},
		{ "doc", "application/msword"},
		{ "dot", "application/msword"},
		{ "wrd", "application/msword"},
		{ "bin", "application/octet-stream"},
		{ "class", "application/octet-stream"},
		{ "ms", "application/octet-stream"},
		{ "oda", "application/oda"},
		{ "pdf", "application/pdf"},
		{ "pgp", "application/pgp-signature"},
		{ "ai", "application/postscript"},
		{ "eps", "application/postscript"},
		{ "ppt", "application/powerpoint"},
		{ "rtf", "application/rtf"},
		{ "wp5", "application/wordperfect5.1"},
		{ "wk", "application/x-123"},
		{ "wz", "application/x-Wingz"},
		{ "bcpio", "application/x-bcpio"},
		{ "z", "application/x-compress"},
		{ "Z", "application/x-compress"},
		{ "cpio", "application/x-cpio"},
		{ "csh", "application/x-csh"},
		{ "deb", "application/x-debian-package"},
		{ "com", "application/x-msdos-program"},
		{ "exe", "application/x-msdos-program"},
		{ "bat", "application/x-msdos-program"},
		{ "dvi", "application/x-dvi"},
		{ "gtar", "application/x-gtar"},
		{ "tgz", "application/x-gtar"},
		{ "gz", "application/x-gzip"},
		{ "hdf", "application/x-hdf"},
		{ "phtml", "application/x-httpd-php"},
		{ "pht", "application/x-httpd-php"},
		{ "php", "application/x-httpd-php"},
		{ "latex", "application/x-latex"},
		{ "hqx", "application/mac-binhex40"},
		{ "frm", "application/x-maker"},
		{ "maker", "application/x-maker"},
		{ "frame", "application/x-maker"},
		{ "fm", "application/x-maker"},
		{ "fb", "application/x-maker"},
		{ "book", "application/x-maker"},
		{ "fbdoc", "application/x-maker"},
		{ "mws", "application/maple-v-r4"},
		{ "mif", "application/x-mif"},
		{ "nc", "application/x-netcdf"},
		{ "cdf", "application/x-netcdf"},
		{ "pl", "application/x-perl"},
		{ "pm", "application/x-perl"},
		{ "sh", "application/x-sh"},
		{ "shar", "application/x-shar"},
		{ "sv4cpio", "application/x-sv4cpio"},
		{ "sv4crc", "application/x-sv4crc"},
		{ "tar", "application/x-tar"},
		{ "tcl", "application/x-tcl"},
		{ "texinfo", "application/x-texinfo"},
		{ "texi", "application/x-texinfo"},
		{ "t", "application/x-troff"},
		{ "tr", "application/x-troff"},
		{ "roff", "application/x-troff"},
		{ "man", "application/x-troff-man"},
		{ "me", "application/x-troff-me"},
		{ "ms", "application/x-troff-ms"},
		{ "ustar", "application/x-ustar"},
		{ "src", "application/x-wais-source"},
		{ "zip", "application/zip"},
		{ "au", "audio/basic"},
		{ "snd", "audio/basic"},
		{ "aif", "audio/x-aiff"},
		{ "aiff", "audio/x-aiff"},
		{ "aifc", "audio/x-aiff"},
		{ "ra", "audio/x-pn-realaudio"},
		{ "ram", "audio/x-pn-realaudio"},
		{ "wav", "audio/x-wav"},
		{ "ief", "image/ief"},
		{ "png", "image/png"},
		{ "tiff", "image/tiff"},
		{ "tif", "image/tiff"},
		{ "ras", "image/x-cmu-raster"},
		{ "pnm", "image/x-portable-anymap"},
		{ "pbm", "image/x-portable-bitmap"},
		{ "pgm", "image/x-portable-graymap"},
		{ "ppm", "image/x-portable-pixmap"},
		{ "rgb", "image/x-rgb"},
		{ "xbm", "image/x-xbitmap"},
		{ "xpm", "image/x-xpixmap"},
		{ "xwd", "image/x-xwindowdump"},
		{ "c", "text/plain"},
		{ "h", "text/plain"},
		{ "cc", "text/plain"},
		{ "hh", "text/plain"},
		{ "cpp", "text/plain"},
		{ "hpp", "text/plain"},
		{ "rtx", "text/richtext"},
		{ "tsv", "text/tab-separated-values"},
		{ "etx", "text/x-setext"},
		{ "vcs", "text/x-vCalendar"},
		{ "vcf", "text/x-vCard"},
		{ "dl", "video/dl"},
		{ "fli", "video/fli"},
		{ "gl", "video/gl"},
		{ "mpeg", "video/mpeg"},
		{ "mpg", "video/mpeg"},
		{ "mpe", "video/mpeg"},
		{ "qt", "video/quicktime"},
		{ "mov", "video/quicktime"},
		{ "avi", "video/x-msvideo"},
		{ "movie", "video/x-sgi-movie"},
		{ "vrm", "x-world/x-vrml"},
		{ "vrml", "x-world/x-vrml"},
		{ "wrl", "x-world/x-vrml"},
		{ NULL, NULL }
};

#ifdef WNDEX
static char	*enclist[10][2] = {
		{ "gz", "x-gzip"},
		{ "z", "x-compress"},
		{ NULL, NULL }
};
#endif
