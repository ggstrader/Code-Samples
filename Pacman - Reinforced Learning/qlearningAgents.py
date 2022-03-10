# qlearningAgents.py
# ------------------
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


from game import *
from learningAgents import ReinforcementAgent
from featureExtractors import *

import random
import util
import math


class QLearningAgent(ReinforcementAgent):
    """
      Q-Learning Agent

      Functions you should fill in:
        - computeValueFromQValues
        - computeActionFromQValues
        - getQValue
        - getAction
        - update

      Instance variables you have access to
        - self.epsilon (exploration prob)
        - self.alpha (learning rate)
        - self.discount (discount rate)

      Functions you should use
        - self.getLegalActions(state)
          which returns legal actions for a state
    """

    def __init__(self, **args):
        ReinforcementAgent.__init__(self, **args)
        self.values = util.Counter()

    def getQValue(self, state, action):
        return self.values[(state, action)]

    def computeValueFromQValues(self, state):
        actions = self.getLegalActions(state)
        if len(actions) == 0:
            return 0.0
        _max = float("-inf")
        for action in actions:
            Qval = self.getQValue(state, action)
            if _max <= Qval or _max == float("-inf"):
                _max = Qval
        return _max

    def computeActionFromQValues(self, state):
        actions = self.getLegalActions(state)
        if len(actions) == 0:
            return None
        _max = float("-inf")
        res = ""
        for action in actions:
            Qval = self.getQValue(state, action)
            if _max <= Qval or _max == float("-inf"):
                _max = Qval
                res = action
        return res

    def getAction(self, state):
        actions = self.getLegalActions(state)
        res = None
        if len(actions) == 0:
            return None
        if util.flipCoin(self.epsilon):
            res = random.choice(actions)
        else:
            res = self.computeActionFromQValues(state)
        return res

    def update(self, state, action, nextState, reward):
        Q, VFQ = self.getQValue, self.computeValueFromQValues
        self.values[(state, action)] = ((1 - self.alpha) * Q(state, action)) + (self.alpha * (reward + self.discount * VFQ(nextState)))

    def getPolicy(self, state):
        return self.computeActionFromQValues(state)

    def getValue(self, state):
        return self.computeValueFromQValues(state)


class PacmanQAgent(QLearningAgent):

    def __init__(self, epsilon=0.05, gamma=0.8, alpha=0.2, numTraining=0, **args):
        args['epsilon'] = epsilon
        args['gamma'] = gamma
        args['alpha'] = alpha
        args['numTraining'] = numTraining
        self.index = 0  # This is always Pacman
        QLearningAgent.__init__(self, **args)

    def getAction(self, state):
        """
        Simply calls the getAction method of QLearningAgent and then
        informs parent of action for Pacman.  Do not change or remove this
        method.
        """
        action = QLearningAgent.getAction(self, state)
        self.doAction(state, action)
        return action


class ApproximateQAgent(PacmanQAgent):

    def __init__(self, extractor='IdentityExtractor', **args):
        self.featExtractor = util.lookup(extractor, globals())()
        PacmanQAgent.__init__(self, **args)
        self.weights = util.Counter()

    def getWeights(self):
        return self.weights

    def getQValue(self, state, action):
        qvalue, features = 0, self.featExtractor.getFeatures(state, action)
        for feature in features:
            qvalue += features[feature] * self.weights[feature]
        return qvalue

    def update(self, state, action, nextState, reward):
        diff = (reward + (self.discount * self.getValue(nextState))) - self.getQValue(state, action)
        features = self.featExtractor.getFeatures(state, action)
        for feature in features:
            self.weights[feature] = self.weights[feature] + (self.alpha * features[feature] * diff)

    def final(self, state):
        "Called at the end of each game."
        PacmanQAgent.final(self, state)
        if self.episodesSoFar == self.numTraining:
            pass
