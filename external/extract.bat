
@rem -----------------------------------------------------------------------------
@rem Extract boost-regex files
@rem -----------------------------------------------------------------------------

set BOOST_REGEX_DEST_DIR=regex-boost-1.92.0
set BOOST_REGEX_FILES=^
 regex-boost-1.92.0/include/^
 regex-boost-1.92.0/src/^
 regex-boost-1.92.0/readme.txt

if not exist %BOOST_REGEX_DEST_DIR%\ (
	mkdir %BOOST_REGEX_DEST_DIR%
	tar -xf regex-boost-1.92.0.zip %BOOST_REGEX_FILES%
)
