#! /bin/sh

export HOME=$SNAP_USER_DATA
export I18NPATH=$SNAP/usr/share/i18n
export LOCPATH=$SNAP_USER_DATA
SNAPLANG=en_US
SNAPENC=UTF-8
SNAPLOC="$SNAPLANG.$SNAPENC"
if [ ! -e $SNAP_USER_DATA/$SNAPLOC ]; then
	localedef --prefix=$SNAP_USER_DATA -f $SNAPENC -i $SNAPLANG $SNAP_USER_DATA/$SNAPLOC
fi

export LC_ALL=$SNAPLOC
export LANG=$SNAPLOC
export LANGUAGE=${SNAPLANG%_*}

$SNAP/usr/local/bin/nms "$@"
