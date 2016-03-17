A   Application   Level   File-Sharing-Protocol   with   support   for   download   and  upload for files and indexed searching. 

Features : 
  1. The   system   has 2 clients   (acting   as   servers   simultaneously)   listening   to   the  communication   channel   for   requests   and   waiting   to   share   files   (avoiding   collisions) using an application layer protocol (like FTP/HTTP). 
  2. Each client has the ability to do the following : 

	- Know   the   files   present on   each   others   machines   in   the   designated   shared  folders. 

	- Download files​ from this shared folder. 

	- Upload files​ to this shared folder. 
	 
  3. File transfer incorporates ​MD5­checksum​ to handle file transfer errors. 
 
Specifications : ​

The system should incorporate the following commands :­ 
 
1.  IndexGet ­­--flag (args) 
	○ can request the display of the shared files on the connected system.  

	○ the ​flag ​variable can be ​shortlist or longlist. 

		1. shortlist   :   ​flag   would   mean   that   the   client   only   wants   to   know   the  names   of   files   between  a   specific   set   of   timestamps.   The   sample   query  is as below. 

			● $> IndexGet ­­shortlist <start­time­stamp> <end­time­stamp> 

			● Output   :   ​should   include   ‘name’,   ‘size’,   ‘timestamp’   and   ‘type’   of  the files between the start and end time stamps. 

		2. longlist   :   ​flag   would   mean   that   client   wants   to   know   the   entire   listing   of  the   shared   folder/directory   including   ‘name’,   ‘size’,   ‘timestamp’   and  ‘type’ of the files. 

			● $> IndexGet ­­longlist 

			● Output : ​similar to above, but with complete file listing. 
		 

2. FileDownload --­­flag (args): 

	○ used   to   download   files   from   the   shared   folder  of connected user to our shared folder. 
	 
		■ $> FileDownload <filename> 

		■ Output   :   ​should   contain   the   filename,   filesize,   last­modified   timestamp  and the MD5­hash of the requested file. 

3. FileUpload --­­flag (args): 

	○ used   to   upload   a   file   to   the   other   clients  shared folder.

		■ $> FileUpload <filename> 

		■ Output   :   ​should   contain   the   filename,   filesize,   last­modified   timestamp  and the MD5­hash of the uploaded file. 
