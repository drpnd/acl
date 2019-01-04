# Algorithms to implement Access Control List

## Convert ACL to TCAM (binary and mask) format

```
$ acl2tcam <ACL>
```

## Generate traffic

Generate traffic based on reverse-order sequential scanning to `192.168.0.0/16`.

* Source address: Random
* Destination address: Reverse-order sequential scanning to `192.168.0.0/16`
* Source port: Random
* Destination port: 5060 (SIP)
* TCP flags: SYN (0x2)

```
$ tgen
```


