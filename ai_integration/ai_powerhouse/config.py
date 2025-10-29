"""
Configuration management for AI Powerhouse
"""

import os
from typing import Optional
from dotenv import load_dotenv
from pydantic import BaseModel


class Config(BaseModel):
    """Configuration settings for AI Powerhouse"""
    
    # API Keys
    anthropic_api_key: Optional[str] = None
    google_api_key: Optional[str] = None
    openai_api_key: Optional[str] = None
    
    # Model settings
    claude_model: str = "claude-3-sonnet-20240229"
    gemini_model: str = "gemini-pro"
    openai_model: str = "gpt-4"
    
    # Generation settings
    max_tokens: int = 4000
    temperature: float = 0.7
    
    @classmethod
    def load_from_env(cls) -> "Config":
        """Load configuration from environment variables"""
        load_dotenv()
        
        return cls(
            anthropic_api_key=os.getenv("ANTHROPIC_API_KEY"),
            google_api_key=os.getenv("GOOGLE_API_KEY"),
            openai_api_key=os.getenv("OPENAI_API_KEY"),
            claude_model=os.getenv("CLAUDE_MODEL", "claude-3-sonnet-20240229"),
            gemini_model=os.getenv("GEMINI_MODEL", "gemini-pro"),
            openai_model=os.getenv("OPENAI_MODEL", "gpt-4"),
            max_tokens=int(os.getenv("MAX_TOKENS", "4000")),
            temperature=float(os.getenv("TEMPERATURE", "0.7"))
        )
    
    def validate_keys(self) -> dict:
        """Validate which API keys are available"""
        available = {}
        available["claude"] = self.anthropic_api_key is not None
        available["gemini"] = self.google_api_key is not None
        available["openai"] = self.openai_api_key is not None
        return available
