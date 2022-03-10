# valueIterationAgents.py
# -----------------------
# Licensing Information:  You are free to use or extend these projects for
# educational purposes provided that (1) you do not distribute or publish
# solutions, (2) you retain this notice, and (3) you provide clear
# attribution to UC Berkeley, including a link to http://ai.berkeley.edu.
#
# Attribution Information: The Pacman AI projects were developed at UC Berkeley.
# The core projects and autograders were primarily created by John DeNero
# (denero@cs.berkeley.edu) and Dan Klein (klein@cs.berkeley.edu).
# Student side autograding was added by Brad Miller, Nick Hay, and
# Pieter Abbeel (pabbeel@cs.berkeley.edu).


# valueIterationAgents.py
# -----------------------
# Licensing Information:  You are free to use or extend these projects for
# educational purposes provided that (1) you do not distribute or publish
# solutions, (2) you retain this notice, and (3) you provide clear
# attribution to UC Berkeley, including a link to http://ai.berkeley.edu.
#
# Attribution Information: The Pacman AI projects were developed at UC Berkeley.
# The core projects and autograders were primarily created by John DeNero
# (denero@cs.berkeley.edu) and Dan Klein (klein@cs.berkeley.edu).
# Student side autograding was added by Brad Miller, Nick Hay, and
# Pieter Abbeel (pabbeel@cs.berkeley.edu).


import mdp as MDP
import util

from learningAgents import ValueEstimationAgent
import collections


class ValueIterationAgent(ValueEstimationAgent):

    def __init__(self, mdp, discount=0.9, iterations=100):
        self.mdp = mdp
        self.discount = discount
        self.iterations = iterations
        self.values = util.Counter()  # A Counter is a dict with default 0
        self.runValueIteration()

    def runValueIteration(self):
        while self.iterations > 0:
            values = self.values.copy()
            for state in self.mdp.getStates():
                chances = []
                for action in self.mdp.getPossibleActions(state):
                    avg = 0
                    for tstate in self.mdp.getTransitionStatesAndProbs(state, action):
                        nextState, prblty = tstate
                        reward = self.mdp.getReward(state, action, nextState)
                        avg += (prblty * (reward + (self.discount * values[nextState])))
                    chances.append(avg)
                if len(chances) != 0:
                    self.values[state] = max(chances)
            self.iterations -= 1

    def computeQValueFromValues(self, state, action):
        avg = 0
        for tstate in self.mdp.getTransitionStatesAndProbs(state, action):
            nextState, prblty = tstate
            reward = self.mdp.getReward(state, action, nextState)
            avg += (prblty * (reward + (self.discount * self.values[nextState])))
        return avg

    def computeActionFromValues(self, state):
        if self.mdp.isTerminal(state):
            return None
        res, _max = "", float("-inf")
        for action in self.mdp.getPossibleActions(state):
            avg = self.computeQValueFromValues(state, action)
            if (_max == 0.0 and action == "") or avg >= _max:
                res, _max = action, avg
        return res

    def getValue(self, state):
        return self.values[state]

    def getPolicy(self, state):
        return self.computeActionFromValues(state)

    def getAction(self, state):
        return self.computeActionFromValues(state)

    def getQValue(self, state, action):
        return self.computeQValueFromValues(state, action)


class AsynchronousValueIterationAgent(ValueIterationAgent):
    def __init__(self, mdp, discount=0.9, iterations=1000):
        ValueIterationAgent.__init__(self, mdp, discount, iterations)

    def runValueIteration(self):
        values, states = self.values.copy(), self.mdp.getStates()
        for i in range(self.iterations):
            state = states[i % len(states)]
            chances, values = [], self.values.copy()
            if not self.mdp.isTerminal(state):
                for action in self.mdp.getPossibleActions(state):
                    avg = 0
                    for tstate in self.mdp.getTransitionStatesAndProbs(state, action):
                        nextState, prblty = tstate
                        reward = self.mdp.getReward(state, action, nextState)
                        avg += prblty * (reward + self.discount * values[nextState])
                    chances.append(avg)
                self.values[state] = max(chances)
            else:
                self.values[state] = 0


class PrioritizedSweepingValueIterationAgent(AsynchronousValueIterationAgent):

    def __init__(self, mdp, discount=0.9, iterations=100, theta=1e-5):
        self.theta = theta
        ValueIterationAgent.__init__(self, mdp, discount, iterations)

    def runValueIteration(self):
        """"""
