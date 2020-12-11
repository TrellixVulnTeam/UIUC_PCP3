import random
import torch
import numpy as np
from collections import deque
import torch.nn as nn
import torch.optim as optim
import torch.nn.functional as F
from torch.autograd import Variable
from memory import ReplayMemory
from model import DQN
from utils import find_max_lives, check_live, get_frame, get_init_state
from config import *
import os

device = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")

class Agent():
    def __init__(self, action_size):
        self.action_size = action_size

        # These are hyper parameters for the DQN
        self.discount_factor = 0.99
        self.epsilon = 1.0
        self.epsilon_min = 0.01
        self.explore_step = 500000
        self.epsilon_decay = (self.epsilon - self.epsilon_min) / self.explore_step
        self.train_start = 100000
        self.update_target = 1000

        # Generate the memory
        self.memory = ReplayMemory()

        # Create the policy net
        self.policy_net = DQN(action_size)
        self.policy_net.to(device)

        self.optimizer = optim.Adam(params=self.policy_net.parameters(), lr=learning_rate)
        self.scheduler = optim.lr_scheduler.StepLR(self.optimizer, step_size=scheduler_step_size, gamma=scheduler_gamma)

    def load_policy_net(self, path):
        self.policy_net = torch.load(path)

    """Get action using policy net using epsilon-greedy policy"""
    def get_action(self, state):
        if np.random.rand() <= self.epsilon:
            ### CODE #### 
            # Choose a random action
            return torch.tensor([[random.randrange(self.action_size)]], device = device, dtype=torch.long)
        else:
            ### CODE ####
            # Choose the best action
            with torch.no_grad():
                state_torch = torch.from_numpy(state).cuda()
                # 4 x 84 x 84
                state_torch = state_torch.unsqueeze(0)
                return self.policy_net(state_torch).max(1)[1].view(1, 1)
                # 4 x 1 x 84 x 84, if unsqueeze 1
                #need 32 4 8 8
                

    # pick samples randomly from replay memory (with batch_size)
    def train_policy_net(self, frame):
        if self.epsilon > self.epsilon_min:
            self.epsilon -= self.epsilon_decay

        mini_batch = self.memory.sample_mini_batch(frame)
        mini_batch = np.array(mini_batch).transpose()

        history = np.stack(mini_batch[0], axis=0)
        states = np.float32(history[:, :4, :, :]) / 255.
        states = torch.from_numpy(states).cuda()
        actions = list(mini_batch[1])
        actions = torch.LongTensor(actions).cuda()
        rewards = list(mini_batch[2])
        rewards = torch.FloatTensor(rewards).cuda()
        next_states = np.float32(history[:, 1:, :, :]) / 255.
        dones = mini_batch[3] # checks if the game is over
        musk = torch.tensor(list(map(int, dones==False)),dtype=torch.bool)

        # Compute Q(s_t, a), the Q-value of the current state
        ### CODE ####
        action_batch = actions.view(-1, 1)
        current_q = self.policy_net(states).gather(1, action_batch)
        
        # Compute Q function of next state
        ### CODE ####
        next_state_value = torch.zeros(batch_size, device=device)
        
        # Find maximum Q-value of action at next state from policy net
        ### CODE ####
        # non_final_next_states = []
        # for idx, s in enumerate(next_states):
        #     if dones[idx] == False:
        #         non_final_next_states.append(s)
                
        # non_final_next_states = torch.FloatTensor(non_final_next_states).cuda()        
        #convert list to tensor
        next_states = torch.from_numpy(next_states).cuda()
        next_state_value[musk] = self.policy_net(next_states[musk]).max(1)[0].detach()
        
        # Compute the Huber Loss
        ### CODE ####
        expected = (next_state_value * self.discount_factor) + rewards
        loss = F.smooth_l1_loss(current_q, expected.unsqueeze(1))
        # Optimize the model, .step() both the optimizer and the scheduler!
        ### CODE ####
        
        self.optimizer.zero_grad()
        loss.backward()
        for param in self.policy_net.parameters():
            param.grad.data.clamp_(-1, 1)
        
        self.optimizer.step()


