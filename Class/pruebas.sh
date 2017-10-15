#!/bin/bash

python resultados.py MDPLib/ ./mdp -r -b &
python resultados.py MDPLib/ ./mdp -c -b &
python resultados.py MDPLib/ ./mdp -d -b &
python resultados.py MDPLib/ ./mdp -c -p 25 &
wait
python resultados.py MDPLib/ ./mdp -d -p 25 &
python resultados.py MDPLib/ ./mdp -r -p 25 &
python resultados.py MDPLib/ ./mdp -r -p 50 &
python resultados.py MDPLib/ ./mdp -c -p 50 &
wait
python resultados.py MDPLib/ ./mdp -d -p 50 &
python resultados.py MDPLib/ ./mdp -r -p 75 &
python resultados.py MDPLib/ ./mdp -c -p 75 &
python resultados.py MDPLib/ ./mdp -d -p 75 &
wait