srb2_protocol = Proto("srb2", "srb2 Protocol")

-- header fields MAXPACKETLENGTH = 1450
checksum = ProtoField.uint32("srb2.checksum","checksum",base.DEC)
ack = ProtoField.uint8("srb2.ack","ack",base.DEC)
ackreturn = ProtoField.uint8("srb2.ackreturn","ackreturn",base.DEC)
packettype = ProtoField.uint8("srb2.packettype","packettype",base.DEC)
reserved = ProtoField.uint8("srb2.reserved","reserved",base.DEC)

--payload fields
client_tic = ProtoField.uint8("srb2.client_tic","client_tic",base.DEC)
resendfrom = ProtoField.uint8("srb2.resendfrom","resendfrom",base.DEC)
consistancy = ProtoField.int16("srb2.consistancy","consistancy",base.DEC)

--cmd fields
forwardmove = ProtoField.int8("srb2.forwardmove","forwardmove",base.DEC)
sidemove = ProtoField.int8("srb2.sidemove","sidemove",base.DEC)
angleturn = ProtoField.int16("srb2.angleturn","angleturn",base.DEC)
aiming = ProtoField.int16("srb2.aiming","aiming",base.DEC)
buttons = ProtoField.uint16("srb2.buttons","buttons",base.DEC)
latency = ProtoField.uint8("srb2.latency","latency",base.DEC)


srb2_protocol.fields = {checksum,ack,ackreturn,packettype,reserved, --header
client_tic,resendfrom,consistancy, -- PT_CLIENTCMD
forwardmove,sidemove,angleturn,aiming,buttons,latency -- cmd
}

function srb2_protocol.dissector(buffer,pinfo,tree)
    length = buffer:len()
    if lengh == 0 then return end
    pinfo.cols.protocol = srb2_protocol.name

    local subtree = tree:add(srb2_protocol,buffer(),"srb2 Data")
    local type_num = buffer(6,1):le_uint()
    local type_name = get_packettype(type_num)
    subtree:add_le(checksum,buffer(0,4))
    subtree:add_le(ack,buffer(4,1))
    subtree:add_le(ackreturn,buffer(5,1))
    subtree:add_le(packettype,buffer(6,1)):append_text("(" ..type_name.. ")")
    subtree:add_le(reserved,buffer(7,1))

    if type_num == 2 then
        local subsubtree = subtree:add(srb2_protocol,buffer(),"PT_CLIENTCMD packet")
        subsubtree:add_le(client_tic,buffer(8,1))
        subsubtree:add_le(resendfrom,buffer(9,1))
        subsubtree:add_le(consistancy,buffer(8,2))
        subsubtree:add_le(forwardmove,buffer(10,1))
        subsubtree:add_le(sidemove,buffer(11,1))
        subsubtree:add_le(angleturn,buffer(12,2))
        subsubtree:add_le(aiming,buffer(14,2))
        subsubtree:add_le(buttons,buffer(16,2))
        subsubtree:add_le(latency,buffer(18,1))
    end
end

function get_packettype(packet_type)
    local typename = "Unknown"

    local type_enum = {
        [0] = "PT_NOTHING",
        [1] = "PT_SERVERCFG",
        [2] = "PT_CLIENTCMD",
        [3] = "PT_CLIENTMIS",
        [4] = "PT_CLIENT2CMD",
        [5] = "PT_CLIENT2MIS",
        [6] = "PT_NODEKEEPALIVE",
        [7] = "PT_NODEKEEPALIVEMIS",
        [8] = "PT_SERVERTICS",
        [9] = "SERVERREFUSE"
    }

    typename = type_enum[packet_type]
    return typename
end

local udp_port = DissectorTable.get("udp.port")
udp_port:add(5029, srb2_protocol)