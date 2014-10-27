/*
    WN: A Server for the HTTP
    File: wn/auth.h
    Version 1.15.7
    
    Copyright (C) 1995, 1996  <by John Franks>

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

#define AUTH_GRANTED	(0)
#define AUTH_DENIED	(1)
#define AUTH_EXPIRED	(2)

static
char * autherr_m[] = {
	/* 0 */ "Authorization module error: error number",
	/* 1 */ "Authorization required and not successful",
	/* 2 */ "",	
	/* 3 */ "Badly formed user info string",
	/* 4 */ "Can't open passwd file",
	/* 5 */ "Can't init dbm file",
	/* 6 */ "Can't open group file",
	/* 7 */ "No password file listed on command line",
	/* 8 */ "DBM code for authorization not installed",
	/* 9 */ "Incorrect or unknown authorization type",
	/* 10 */ "No AUTHORIZATION header",
	/* 11 */ "Authorization module failed",
	/* 12 */ "Improper usage: bad options",
	/* 13 */ "Authorization denied",
	/* 14 */ "Authorization expired",
	/* 15 */ "",
	/* 16 */ "Authentication timed out",
    /* 17 */ "Logged off"
};

	
