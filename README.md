Socklab
=======

Socklab is an interactive tool to learn about the socket API for UDP and TCP,
without having to actually write C code.  The original description in French
was "laboratoire d'étude des sockets Internet".

It allows to create UDP and TCP sockets, `bind()` and `connect()` them, `read()`,
`write()` to them, etc.  All this is provided by a shell-like interface with
auto-completion.

Socklab is known to work fine on Linux, FreeBSD and OS X.  It has been used extensively
to teach networking courses over the last 30 years:

- Université Grenoble Alpes / Grenoble INP / Polytech Grenoble
- Université Savoie Mont Blanc / IUT Annecy
- Université de La Rochelle
- [Université Lille 1 / Faculté des Sciences et Technologies / MIAGE](https://www.fil.univ-lille1.fr/~noe/rsx1_portail/files/TP1.pdf)
- [Université de Caen Normandie / IUT Caen](https://loudni.users.greyc.fr/Enseignement/Cours/TR2/)
- [Université de la Réunion](http://lim.univ-reunion.fr/staff/panelli/4_teaching/4_TP-RX/index.html)

Using `socklab`
---------------

You can run socklab in either UDP mode, TCP mode, or an advanced "standard" mode
that allows to use both UDP and TCP:

```
$ socklab udp
$ socklab tcp
$ socklab
```

Once socklab is started (whatever the mode), run `help` to get a list of available commands.

Documentation
-------------

There are several sources of documentation:

- basic help: available within socklab itself by running `help`
- description of commands: man page `socklab(1)`
- most complete documentation: the LaTeX documentation in `doc/` (in French)

Building
--------

The only dependency necessary to build socklab is `readline`.  You should install
the `libreadline-dev` package on Debian-based systems.

Socklab uses a simple Makefile, so it should be enough to do:

    $ make

On BSD, make sure to use `gmake`:

    $ gmake

For packagers, the `install` target of the Makefile supports the standard GNU
variables (`prefix`, `DESTDIR`, etc).

The code is formatted with GNU indent, using a style similar to the Linux kernel.
The following GNU indent options are used:

    -nbad -bap -nbc -bbo -hnl -br -brs -c33 -cd33 -ncdb -ce -ci4 
    -cli0 -d0 -di1 -nfc1 -i4 -ip0 -l150 -lp -npcs -nprs -npsl -sai
    -saf -saw -ncs -nsc -sob -nfca -cp33 -ss -nut -il1

License
-------

Socklab uses the CeCILL V2 license, see `Licence_CeCILL_V2-en.txt`.

Sous Licence CeCILL V2, voir le fichier `Licence_CeCILL_V2-fr.txt`.

How to make a new release
-------------------------

- determine the new version number using [semantic versioning](https://semver.org/)
- update `VERSION` in the Makefile
- create a new entry in the `CHANGELOG` file
- commit in git
- create tag: `git tag v1.x.y`
- push everything with `git push && git push --tags`
- create a release on [github](https://github.com/drakkar-lig/socklab/releases) and copy-paste the changelog entry

Contributors
------------

Developped initially by L. Croq, L. Sali, P. Sicard.

Successive contributions L. Bellissard, M. Heusse, F. Rousseau, B. Jonglez, and others.
