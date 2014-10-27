# -------------------------------------------------------------------------
#
# You are granted a limited copyright to modify and use this sample
# code for your internal purposes only. THIS SOFTWARE IS PROVIDED "AS-
# IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
# NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
# PARTICULAR PURPOSE OR NONINFRINGEMENT. IN NO EVENT SHALL THE
# HEWLETT-PACKARD COMPANY OR ITS SUBSIDIARIES BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
# OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
# SOFTWARE INCURRED BY YOU OR ANY THIRD PARTY IN CONNECTION WITH THE
# USE OF THIS SOFTWARE.
#
# -------------------------------------------------------------------------   

JAVA_HOME=/usr/tandem/nssjava/jdk141_v10

#Installation subvolume
#Specify the absolute path name of this directory
INSTALLDIR =

#Specify DICTPATH using the /G syntax
#Example: G_DICTPATH = /G/data11/jpathsnd
G_DICTPATH =

#Specify the same value as for G_DICTPATH for G_PATHWAY_SUBVOL only use GUARDIAN syntax.
#Note that the $ is escaped in the following example
#Example: G_PATHWAY_SUBVOL = \$$data11.jpathsnd
G_PATHWAY_SUBVOL =

#Specify a pathmon name - this name must begin with a dollar sign followed by one to
#five alphanumeric characters; the first of which must be an alpha character.
#Note that the $ is escaped in the following example:
#Example: PATHMON_NAME = \$$PMON
PATHMON_NAME =
PRIMARY_CPU = 0
BACKUP_CPU = 1

#----------------------------------------------------------------------------
#-- You shouldn't have to edit anything below this line
L_PATHWAY_SUBVOL = ${G_DICTPATH}

all:	${G_DICTPATH}/zjpath \
	${L_PATHWAY_SUBVOL}/employ \
	${L_PATHWAY_SUBVOL}/jpctl 
	

${G_DICTPATH}/zjpath: 
	rm -f empddl1
	rm -f empddl
	rm -f empddl2
	echo "?dict $(G_PATHWAY_SUBVOL)" >> empddl
	echo "delete record employee." >> empddl
	echo "delete def employee-reply." >> empddl
	echo "delete def employee-add." >> empddl
	echo "delete def employee-request." >> empddl
	echo "delete def error-reply." >> empddl
	echo "definition error-reply." >> empddl
	echo "02 reply-code  pic 99." >> empddl
	echo "02 error-text  pic x(60)." >> empddl
	echo "end" >> empddl
	echo "definition employee-def." >> empddl
	echo "02 employee-number pic 9999." >> empddl
	echo "02 empname." >> empddl
	echo "03 first pic x(20)." >> empddl
	echo "03 last pic x(30)." >> empddl
	echo "03 middle pic x." >> empddl
	echo "02 regnum pic 99." >> empddl
	echo "02 branchnum pic 99." >> empddl
	echo "end" >> empddl
	echo "definition employee-add." >> empddl
	echo "02 request-code pic 99." >> empddl
	echo "02 employee-info type employee-def." >> empddl
	echo "end" >> empddl
	echo "definition employee-reply." >> empddl
	echo "02 reply-code pic 99." >> empddl
	echo "02 employee-info type employee-def." >> empddl
	echo "end" >> empddl
	echo "record employee." >> empddl
	echo "file is employ key-sequenced." >> empddl
	echo "02 employ type employee-def." >> empddl
	echo "key is employee-number duplicates not allowed." >> empddl
	echo "end" >> empddl
	echo "definition employee-request." >> empddl
	echo "02 request-code pic 99." >> empddl
	echo "02 employee-number pic 9999." >> empddl
	echo "end" >> empddl
	

${L_PATHWAY_SUBVOL}/employ: 
	rm -f empfup
	
	echo "allow 100 severe errors" > empfup
	echo "volume ${G_PATHWAY_SUBVOL}" >> empfup
	echo "alter employ, no audit" >> empfup
	echo "purge employ !" >> empfup
	echo "RESET" >> empfup
	echo "SET TYPE K" >> empfup
	echo "SET KEYLEN 4" >> empfup
	echo "SET REC 59" >> empfup
	echo "SET BLOCK 4096" >> empfup
	echo "SET IBLOCK 4096" >> empfup
	echo "SET EXT(4, 32)" >> empfup
	echo "SET AUDIT" >> empfup
	echo "SET MAXEXTENTS 100" >> empfup
	echo "CREATE employ" >> empfup
	

${L_PATHWAY_SUBVOL}/jpctl:

	gtacl -cv "purge ${G_PATHWAY_SUBVOL}.jobey"
	rm -f editin
	echo "get ${G_PATHWAY_SUBVOL}.jobey !" > editin
	echo "add" >> editin
	echo "volume ${G_PATHWAY_SUBVOL}" >> editin
	echo "fup purge jplog !" >> editin
	echo "fup create jplog, maxextents 100" >> editin
	echo "assign pathctl, ${G_PATHWAY_SUBVOL}.jpctl" >> editin
	echo "pathmon/name ${PATHMON_NAME}, cpu ${PRIMARY_CPU}, nowait, out jplog/${BACKUP_CPU}" >> editin
	echo "//" >> editin
	echo "exit" >> editin


	rm -f pathcnfg
	echo "set pathmon backupcpu ${BACKUP_CPU}" > pathcnfg
	echo "set pathmon dump on" >> pathcnfg
	echo "set pathway maxassigns 10" >> pathcnfg
	echo "set pathway maxdefines 10" >> pathcnfg
	echo "set pathway maxlinkmons 10" >> pathcnfg
	echo "set pathway maxparams 10" >> pathcnfg
	echo "set pathway maxserverclasses 10" >> pathcnfg
	echo "set pathway maxserverprocesses 30" >> pathcnfg
	echo "set pathway maxstartups 10" >> pathcnfg
	echo "set pathway maxtcps 2" >> pathcnfg 
	echo "set pathway maxterms 2" >> pathcnfg
	echo "set pathway security \"N\"">> pathcnfg
	echo "start pathway cold !" >> pathcnfg
	echo "reset server" >> pathcnfg
	echo "set server processtype oss" >> pathcnfg
	echo "set server autorestart 3" >> pathcnfg
	echo "set server cpus(${BACKUP_CPU}:${PRIMARY_CPU})" >> pathcnfg
	echo "set server createdelay 1 mins" >> pathcnfg
	echo "set server cwd ${INSTALLDIR}" >> pathcnfg
	echo "set server deletedelay 30 mins" >> pathcnfg
	echo "set server highpin off" >> pathcnfg
	echo "set server linkdepth 1" >> pathcnfg
	echo "set server maxservers 5" >> pathcnfg
	echo "set server numstatic 1" >> pathcnfg
	echo "set server tmf on" >> pathcnfg
	echo "set server program ${INSTALLDIR}/empsvr" >> pathcnfg
	echo "set server volume ${G_PATHWAY_SUBVOL}" >> pathcnfg
	echo "add server empsvr" >> pathcnfg

		
clean:	
	rm -f empfup
	rm -f editin
	rm -f DICTSUB
	rm -f pathcnfg

clobber: 
	rm -f empfup
	rm -f editin
	rm -f DICTSUB
	rm -f dictsub
	rm -f pathcnfg
	rm -f employeeInfo.class
	rm -f employee_class/*
	rm -f employee_src/*
	rm -rf employee_class
	rm -rf employee_src
	gtacl -cv "purge ${G_PATHWAY_SUBVOL}.employ"
	gtacl -cv "purge ${G_PATHWAY_SUBVOL}.jplog"	
	gtacl -p pathcom ${PATHMON_NAME} < pshutdown
	gtacl -cv "purge ${G_PATHWAY_SUBVOL}.jpctl"

