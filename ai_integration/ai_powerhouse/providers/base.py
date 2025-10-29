"""
Base provider interface for AI services
"""

from abc import ABC, abstractmethod
from typing import Dict, Any, Optional


class BaseProvider(ABC):
    """Abstract base class for AI providers"""
    
    def __init__(self, api_key: str, model: str, **kwargs):
        self.api_key = api_key
        self.model = model
        self.config = kwargs
    
    @abstractmethod
    async def generate_response(self, prompt: str, **kwargs) -> str:
        """Generate a response from the AI provider"""
        pass
    
    @abstractmethod
    def validate_connection(self) -> bool:
        """Validate the connection to the AI provider"""
        pass
    
    @property
    @abstractmethod
    def provider_name(self) -> str:
        """Return the name of the provider"""
        pass
    
    def get_provider_info(self) -> Dict[str, Any]:
        """Get information about the provider"""
        return {
            "name": self.provider_name,
            "model": self.model,
            "available": self.validate_connection()
        }
