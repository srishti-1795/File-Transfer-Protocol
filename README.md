A   Application   Level   File-Sharing-Protocol   with   support   for   download   and  upload for files and indexed searching. 

Features : 
  1. The   system   should   have 2 clients   (acting   as   servers   simultaneously)   listening   to   the  communication   channel   for   requests   and   waiting   to   share   files   (avoiding   collisions) using an application layer protocol (like FTP/HTTP). 
  2. Each client has the ability to do the following : 
	○ Know   the   files   present on   each   others   machines   in   the   designated   shared  folders. 
	○ Download files​ from this shared folder. 
	○ Upload files​ to this shared folder. 
	 
  3. The system should ​periodically check​ for any changes made to the shared folders. 
  4. File transfer should incorporate ​MD5­checksum​ to handle file transfer errors. 
 
Specifications : ​
The system should incorporate the following commands :­ 
 
 -  IndexGet ­­--flag (args) 
	○ can request the display of the shared files on the connected system. 
	○ the   history   of   requests   made   by   either   clients   should   be   maintained   at   each   of  the clients respectively. 
	○ the ​flag ​variable can be ​shortlist, longlist or regex. 
		1. shortlist   :   ​flag   would   mean   that   the   client   only   wants   to   know   the  names   of   files   between  a   specific   set   of   timestamps.   The   sample   query  is as below. 
			● $> IndexGet ­­shortlist <start­time­stamp> <end­time­stamp> 
			● Output   :   ​should   include   ‘name’,   ‘size’,   ‘timestamp’   and   ‘type’   of  the files between the start and end time stamps. 
		2. longlist   :   ​flag   would   mean   that   client   wants   to   know   the   entire   listing   of  the   shared   folder/directory   including   ‘name’,   ‘size’,   ‘timestamp’   and  ‘type’ of the files. 
			● $> IndexGet ­­longlist 
			● Output : ​similar to above, but with complete file listing. 
		
 
  - FileHash --­­flag (args) : 
	○ this   command   indicates   that   the   client   wants   to   check   if   any   of   the   files   on   the  other   end   have   been   changed.   The   flag   variable   can   take   two   values,  verify  and checkall​ : 
		1. verify   : flag   should   check   for   the   specific   file   name   provided   as  command   line   argument   and return   its   ‘checksum’   and   ‘last­modified’  timestamp. 
			● $> FileHash ­­verify <filename> 
			● Output : ​checksum and last­modified timestamp of the input file. 
		 checkall   :   ​flag   should   check   perform   what   ‘verify’   does   for   all   the   files   in  the shared folder. 
			● $> FileHash ­­checkall 
			● Output   :   ​filename,   checksum   and   last­modified   timestamp   of   all  the files in the shared directory. 

  - FileDownload --­­flag (args): 
	○ as   the   name   suggests,   would   be   used   to   download   files   from   the   shared   folder  of connected user to our shared folder. 
	○ If   a   socket   is   not   available,   it   should   be   created   and   both   clients   must   use   this  socket for file transfer. 
		■ $> FileDownload <filename> 
		■ Output   :   ​should   contain   the   filename,   filesize,   last­modified   timestamp  and the MD5­hash of the requested file. 

  - FileUpload --­­flag (args): 
	○ as   the   name   suggests,   would   be   used   to   upload   a   file   to   the   other   clients  shared folder. 
	○ The   other   client   side   may   respond   to   this   request   with   a   ​FileUploadDeny   ​or FileUploadAllow  request   which   would   cancel   or   proceed   with   the   transfer,  respectively. 
	○ The   client   should   upload   the   file,   its   MD5­hash   and   last­modified   timestamp   if  the   response   is FileUploadAllow   and   go   back   to   listening   to   the   communication  channel if the response if FileUploadDeny. 

	○ If   a   socket   is   not   available,   it   should   be   created   and   both   clients   must   use   this socket for file transfer. 
		■ $> FileUpload <filename> 
		■ Output   :   ​should   contain   the   filename,   filesize,   last­modified   timestamp  and the MD5­hash of the uploaded file. 
