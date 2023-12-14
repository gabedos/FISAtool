# Tool for Interpretting FISA S 1801

[FISA S 1801(f)](https://www.law.cornell.edu/uscode/text/50/1801)

“Electronic surveillance” is defined as:

    (1)the acquisition by an electronic, mechanical, or other surveillance device of the contents of any wire or radio communication sent by or intended to be received by a particular, known United States person who is in the United States, if the contents are acquired by intentionally targeting that United States person, under circumstances in which a person has a reasonable expectation of privacy and a warrant would be required for law enforcement purposes;

    (2)the acquisition by an electronic, mechanical, or other surveillance device of the contents of any wire communication to or from a person in the United States, without the consent of any party thereto, if such acquisition occurs in the United States, but does not include the acquisition of those communications of computer trespassers that would be permissible under section 2511(2)(i) of title 18;

    (3)the intentional acquisition by an electronic, mechanical, or other surveillance device of the contents of any radio communication, under circumstances in which a person has a reasonable expectation of privacy and a warrant would be required for law enforcement purposes, and if both the sender and all intended recipients are located within the United States; or

    (4)the installation or use of an electronic, mechanical, or other surveillance device in the United States for monitoring to acquire information, other than from a wire or radio communication, under circumstances in which a person has a reasonable expectation of privacy and a warrant would be required for law enforcement purposes.


In order the capture these fine details of the law, we used a first order logic theorem prover known as [SPASS](https://www.mpi-inf.mpg.de/departments/automation-of-logic/software/spass-workbench/classic-spass-theorem-prover). Through a fine reading of the law, we converted this section into a series of first-order logic predicates to build a formula that would be able to determine whether an action is permissible under FISA. This formula is incorporated in the Flask backend and ultimately display results on the user webpage.
