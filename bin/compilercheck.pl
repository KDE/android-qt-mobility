#!/usr/bin/perl
#############################################################################
##
## Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
## All rights reserved.
## Contact: Nokia Corporation (qt-info@nokia.com)
##
## This file is part of the Qt Mobility Components.
##
## $QT_BEGIN_LICENSE:LGPL$
## Commercial Usage
## Licensees holding valid Qt Commercial licenses may use this file in 
## accordance with the Qt Commercial License Agreement provided with
## the Software or, alternatively, in accordance with the terms
## contained in a written agreement between you and Nokia.
##
## GNU Lesser General Public License Usage
## Alternatively, this file may be used under the terms of the GNU Lesser
## General Public License version 2.1 as published by the Free Software
## Foundation and appearing in the file LICENSE.LGPL included in the
## packaging of this file.  Please review the following information to
## ensure the GNU Lesser General Public License version 2.1 requirements
## will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
##
## In addition, as a special exception, Nokia gives you certain additional
## rights.  These rights are described in the Nokia Qt LGPL Exception
## version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
##
## GNU General Public License Usage
## Alternatively, this file may be used under the terms of the GNU
## General Public License version 3.0 as published by the Free Software
## Foundation and appearing in the file LICENSE.GPL included in the
## packaging of this file.  Please review the following information to
## ensure the GNU General Public License version 3.0 requirements will be
## met: http://www.gnu.org/copyleft/gpl.html.
##
## If you are unsure which license is appropriate for your use, please
## contact the sales department at qt-sales@nokia.com.
## $QT_END_LICENSE$
##
#############################################################################

# use module

my $result = `sbs --query=config[armv5_urel] 2>&1`;
my $count = length($result);
my $idx = index($result, "arm.v5.urel.rvct");

print "check sbs release armv5 default compiler...\n";
if ($idx > 0 && $count > ($idx + 20)){
	$version_hig = substr($result, $idx+16, 1);
	$version_low = substr($result, $idx+18, 1);
	print "- sbs default release armv5 compiler found: RVCT" . $version_hig . "." . $version_low . "\n";

	# check if RVCT in sbs config is < 4.0
	if($version_hig < 4 && $version_hig >= 2){
		#check installed ARM compiler version
		$result = `armcc 2>&1`;
		if( index($result, "RVCT") > 0){
			$idx = index($result, ".");
			$rvctversion_hig = substr($result, $idx-1, 1);
			$rvctversion_low = substr($result, $idx+1, 1);
			print "- installed RVCT compiler: RVCT". $rvctversion_hig . "." . $rvctversion_low . "\n";
			#check if installed RVCT is >= 4.0
			if( $rvctversion_hig >= 4){
				#check if the variable QT_RVCT_VERSION is set to the rvct version
				$qtvarval = $ENV{'QT_RVCT_VERSION'};
				$qtvarshould = $rvctversion_hig. "." .$rvctversion_low;
				if((length($qtvarval) <= 0) || ($qtvarshould != $qtvarval)){
					print "- Please set enviroment variable QT_RVCT_VERSION=";
					print "$rvctversion_hig.$rvctversion_low and run configure again!!!\n";
				}
			}
		}
		else{
			print "- OK\n";
		}
	}
}
else{
    print "- OK\n";
}
