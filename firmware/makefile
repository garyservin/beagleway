# Copyright 2011 Adam Green (http://mbed.org/users/AdamGreen/)
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

PROJECT=Beagleway
GCC4MBED_DIR=/home/gary/src/gcc4mbed
INCDIRS=./libraries
LIBS_PREFIX=
LIBS_SUFFIX=

ifdef LPC_UPLOAD
UPLOAD_COMMAND = $(subst PROJECT,$(PROJECT),$(LPC_UPLOAD))
upload:
	$(UPLOAD_COMMAND)
endif

ifdef LPC_RESET
reset:
		$(LPC_RESET)
endif

include $(GCC4MBED_DIR)/build/gcc4mbed.mk
