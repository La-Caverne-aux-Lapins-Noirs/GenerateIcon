#
# Jason Brillante "Damdoshi"
# Hanged Bunny Studio 2014-2022
# Pentacle Technologie 2008-2022
#
# Generate Icon
#

PROGRAM_DIR	=	/usr/bin/
RESSOURCE_DIR	=	/opt/technocore/genicon/

all:
install:
	mkdir -p $(PROGRAM_DIR)
	mv src/genicon.php $(PROGRAM_DIR)/genicon

	mkdir -p $(RESSOURCE_DIR)
	cp res/* $(RESSOURCE_DIR)
