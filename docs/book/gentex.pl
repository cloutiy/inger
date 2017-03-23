
# get the current direcotry, and move two directories down.

# $dirname = ".";
# opendir(DIR, $dirname) or die "can't opendir $dirname: $!";
# while (defined($file = readdir(DIR))) {
    # Only files that end with .c or .h are interesting,
    # and directories (we recurse through them).
    
    # do something with "$dirname/$file"
# }
# closedir(DIR);  

sub RemoveTrailingSpace
{
    $str = shift;
    while( substr( $str, -1 ) eq " " or substr( $str, 1 ) eq "\t" or substr($str, -1) eq "\n" or substr( $str, -1 ) eq "\r" )
    {
        $str = substr( $str, 0, length($str)-1 );
    }
    return( $str );
}

sub RemoveWhitespace
{
    $str = shift;
    while( substr( $str, 0, 1 ) eq " " or substr( $str, 0, 1 ) eq "\t" ) 
    {
        $str = substr( $str, 1, length($str)-1 );
    }
    while( substr( $str, -1 ) eq " " or substr( $str, 1 ) eq "\t" or substr($str, -1) eq "\n" or substr( $str, -1 ) eq "\r" )
    {
        $str = substr( $str, 0, length($str)-1 );
    }
    return( $str );
}


use File::Find;

# Find all .c and .h files in ../.., and find the tags
# in those files. Store them in %hash.
find( sub {
    # Process only .c and .h files.
    $File::Find::name, -d && '/' ;
    # Grab last two chars of filename. Must be .c or .h
    $i = 0;
    do
    {
      $i = $i + 1;
      $ext= substr( $File::Find::name, -$i );
      $s = substr( $ext, 0, 1 );
    }
    while( $s ne "." and $i < $File::File::name.length );
    $ext = substr( $File::Find::name, -$i+1 );
    # print $ext; print "\n";
    
    if( ( $ext eq "i" or $ext eq "h" or $ext eq "ih"  or $ext eq "c" or $ext eq "l" or $ext eq "b") and $_ ne "lexer.c" )
    {
      $name = $_;
      # Open each file and search for tags.
      open( SOURCE, $name ) or die "Could not open $name: $!";
      while( defined( $line = <SOURCE> ) )
      {
         # Check each line for a tag.
         if( $line =~ /( \t)*\/\*![_A-Za-z]+\*\// )
         {
           $line = RemoveWhitespace $line;
           $tag = substr( $line, 3, length( $line ) - 5 );
	   # print "Adding tag $tag from $name\n";
           $hash{ $tag } = $File::Find::name;
         }
      }
      close( SOURCE );
   } 
},   "../.." );  

# --- MAIN ---
print "This is GENTEX.\n";

# For each TeX file in the current directory (.)
opendir(DIR, ".") or die "can't opendir .: $!";
while (defined($file = readdir(DIR))) 
{
  # Filter out non-TeX files
  $ext = substr( $file, -4 );
  if( $ext eq ".otx" )
  {
    print "Processing $file "; 

    # output file = input file - .otx + .tex
    $output = substr( $file, 0, length( $file ) - 4 );
    $output = $output . ".tex";

    # For each otx file, scan for tags
    open( TEX, $file ) or die "Could not open $file: $!";
    open( OUTPUT, "> $output" ) or die "Could not open $output for writing: $!";
    while( defined( $line = <TEX> ) )
    {
        if( $line =~ /%( \t)*![A-Za-z]+/ )
        {
            # find TAG in hash, die if not found.
            $line = RemoveWhitespace $line;
            $tag = substr( $line, 2, length( $line ) - 2 );
            print "[$tag] ";
            if( $hash{ $tag } eq undef )
            {
                print "(UNDEFINED!) ";
            }
            else
            {
                # START LATEX CODE
                # print OUTPUT "\\begin{listing}\n\\begin{verbatim}\n";

                # open source C file, and find tag.
                open( SOURCE, $hash{ $tag } ) or die "Could not open $hash{ $tag } for reading: $!";
                $copy = 0;
                while( defined( $line = <SOURCE> )  )
                {
                    $wline = RemoveWhitespace $line;
                    if( $wline eq "\/\*!$tag\*\/" )
                    { 
                      $copy = 1;
                    }
                    elsif( $wline eq "\/\*!\*\/" )
                    {
                      $copy = 0;
                    }
                    else
                    {
                      if( $copy == 1 )
                      {
                        $line = RemoveTrailingSpace $line;
                        print OUTPUT $line; print OUTPUT "\n";
                      }
                    }
                }       
                close( SOURCE ); 
 
                # END LATEX CODE
                # print OUTPUT "\\end{verbatim}\n\\end{listing}\n";
            }
    
        }
        else
        {
            print OUTPUT $line;
        }
    }
    close( OUTPUT );
    close( TEX );
    print "\n";
  }
    
}
closedir(DIR);  

print "End GENTEX.\n";


