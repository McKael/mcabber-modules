#! /bin/sh
#
# Copyright (C) 2010 Mikael Berthe <mikael@lilotux.net>
#
# Copy this script on your system and specify the path for mcabber
# by setting the 'extsay_script_path' option.
#
# This script is free software.
# MiKael, 2010-04-02

FIFOPATH="$HOME/.mcabber/mcabber.fifo"

tmpdir=${TMPDIR:=$TMP}
tmpdir=${tmpdir:="/tmp"}
editor=${EDITOR:="vi"}

jid="."

# Use argument as a recipient JID, if it is provided
[ $# -eq 1 ] && jid=$1

# Leave if the FIFO is not available
[ -p $FIFOPATH ] || exit 255

tf=$(mktemp $tmpdir/extsay-XXXXXX) || exit 255

# This will not work if the editor runs in the background!
$editor $tf

# Send the message using MCabber's pipe
if [ -s $tf ]; then
    cmd="say_to -f $tf $jid"
else
    cmd="echo [extsay] The file has not been modified.  Message cancelled."
fi
echo $cmd >> $FIFOPATH

# Do not remove the file too soon
setsid sh -c "cd / && sleep 20 && rm $tf & :" /dev/null 2>&1 < /dev/null

exit 0
