require linux-gta01.inc

SRC_URI += "svn://svn.openmoko.org/branches/src/target/kernel/2.6.22.x;module=patches;proto=http"
#SRC_URI += "file://fix-EVIOCGRAB-semantics.patch;patch=1"

MOKOR = "moko10"
PR = "${MOKOR}-r1"

VANILLA_VERSION = "2.6.22.1"

