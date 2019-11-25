#
# Executes commands at the start of an interactive session.
#
# Authors:
#   Sorin Ionescu <sorin.ionescu@gmail.com>
#

# Source Prezto.
if [[ -s "${ZDOTDIR:-$HOME}/.zprezto/init.zsh" ]]; then
  source "${ZDOTDIR:-$HOME}/.zprezto/init.zsh"
fi

# Customize to your needs...

alias keio="ssh -l ub301471 login00.user.keio.ac.jp"
alias keiox="ssh -X -l ub301471 login00.user.keio.ac.jp"
alias la="ls -al"

function gccdo() {
    gcc $1;
    ./a.out;
    rm -rf ./a.out;
}
function ksend(){
    scp -r $1 ub301471@login00.user.keio.ac.jp:~/sent
}

function uni() {
	make
	./bufcache
	rm bufcache
}

function search() {
	grep -iRnw "$1" .
}

#[ -f ~/.fzf.zsh ] && source ~/.fzf.zsh

PS1='\w: ' 

[ -f ~/.fzf.bash ] && source ~/.fzf.bash

#database
export PGDATA=/usr/local/var/postgres

alias robot="ssh ri@192.168.0.122"

export CLICOLOR=1
export LSCOLORS="GxFxCxDxBxegedabagaced"
