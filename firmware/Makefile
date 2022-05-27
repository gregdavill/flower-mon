TARGET_EXEC ?= flower.elf

AS := sdas8051
CC := sdcc
LD := sdld

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src \
			./vendor/zstack-lib \
			./vendor/BME280_driver \
			./vendor/Z-Stack_3.0.2/Projects/zstack/HomeAutomation/Source \
			./vendor/Z-Stack_3.0.2/Projects/zstack/ZMain/TI2530DB \
			./vendor/Z-Stack_3.0.2/Components/hal/include \
            ./vendor/Z-Stack_3.0.2/Components/hal/target/CC2530EB \
            ./vendor/Z-Stack_3.0.2/Components/mac/include \
            ./vendor/Z-Stack_3.0.2/Components/mac/high_level \
            ./vendor/Z-Stack_3.0.2/Components/mac/low_level/srf04 \
            ./vendor/Z-Stack_3.0.2/Components/mac/low_level/srf04/single_chip \
            ./vendor/Z-Stack_3.0.2/Components/mt \
            ./vendor/Z-Stack_3.0.2/Components/osal/include \
            ./vendor/Z-Stack_3.0.2/Components/services/saddr \
            ./vendor/Z-Stack_3.0.2/Components/services/sdata \
            ./vendor/Z-Stack_3.0.2/Components/stack/af \
            ./vendor/Z-Stack_3.0.2/Components/stack/bdb \
            ./vendor/Z-Stack_3.0.2/Components/stack/GP \
            ./vendor/Z-Stack_3.0.2/Components/stack/nwk \
            ./vendor/Z-Stack_3.0.2/Components/stack/sapi \
            ./vendor/Z-Stack_3.0.2/Components/stack/sec \
            ./vendor/Z-Stack_3.0.2/Components/stack/sys \
            ./vendor/Z-Stack_3.0.2/Components/stack/zcl \
            ./vendor/Z-Stack_3.0.2/Components/stack/zdo \
            ./vendor/Z-Stack_3.0.2/Components/zmac \
            ./vendor/Z-Stack_3.0.2/Components/zmac/f8w \
             
		

SRCS := $(shell find $(SRC_DIRS) -name *.c -or -name *.s51)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)

#INC_DIRS += "./vendor/Z-Stack_3.0.2/Projects/zstack/ZMain/TI2530DB

INC_FLAGS := $(addprefix -I,$(INC_DIRS)) -I/usr/share/sdcc/include -I/usr/share/sdcc/lib

FLAGS ?= -D__GNUC__ -DCPU32MHZ -DROOT=__near_func -DMAC_CFG_TX_DATA_MAX=3 -DMAC_CFG_TX_MAX=6 -DMAC_CFG_RX_MAX=3 -DZIGBEEPRO -DSECURE=1 -DZG_SECURE_DYNAMIC=0 -DREFLECTOR -DDEFAULT_CHANLIST=0x07FFF800 -DZDAPP_CONFIG_PAN_ID=0xFFFF -DNWK_START_DELAY=100 -DEXTENDED_JOINING_RANDOM_MASK=0x007F -DBEACON_REQUEST_DELAY=100 -DBEACON_REQ_DELAY_MASK=0x00FF -DLINK_STATUS_JITTER_MASK=0x007F -DROUTE_EXPIRY_TIME=30 -DAPSC_ACK_WAIT_DURATION_POLLED=3000 -DNWK_INDIRECT_MSG_TIMEOUT=7 -DMAX_RREQ_ENTRIES=8 -DAPSC_MAX_FRAME_RETRIES=3 -DNWK_MAX_DATA_RETRIES=2 -DMAX_POLL_FAILURE_RETRIES=2 -DMAX_BCAST=9 -DAPS_MAX_GROUPS=16 -DMAX_RTG_ENTRIES=40 -DNWK_MAX_BINDING_ENTRIES=5 -DMAX_BINDING_CLUSTER_IDS=15 -DDEFAULT_KEY="{0}" -DMAC_MAX_FRAME_SIZE=116 -DZDNWKMGR_MIN_TRANSMISSIONS=20 -DCONST="const __code" -DGENERIC=__generic -DRFD_RCVC_ALWAYS_ON=FALSE -DPOLL_RATE=1000 -DQUEUED_POLL_RATE=100 -DRESPONSE_POLL_RATE=100 -DREJOIN_POLL_RATE=440 -DREJOIN_BACKOFF=900000 -DREJOIN_SCAN=900000 -DENABLE_LED4_DISABLE_S1 \
 -lm

ASFLAGS ?= $(FLAGS) $(INC_FLAGS)
CPPFLAGS ?=  $(FLAGS) $(INC_FLAGS)
#LDFLAGS ?= $(FLAGS) -T ./vendor/Ld/Link.ld -nostartfiles -Xlinker --gc-sections -Wl,-Map,"$(BUILD_DIR)/CH32V307VCT6.map" --specs=nano.specs --specs=nosys.specs


$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# assembly
$(BUILD_DIR)/%.s51.o: %.s51
	$(MKDIR_P) $(dir $@)
	$(CC) $(ASFLAGS) -c $< -o $@

# c source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p
