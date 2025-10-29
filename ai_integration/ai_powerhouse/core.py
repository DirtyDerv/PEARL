"""
Core AI Powerhouse class that orchestrates multiple AI providers
"""

import asyncio
from typing import Dict, List, Optional, Any
from .config import Config
from .providers import ClaudeProvider, GeminiProvider, OpenAIProvider


class AIPowerhouse:
    """Main class that coordinates multiple AI providers"""
    
    def __init__(self, config: Optional[Config] = None):
        self.config = config or Config.load_from_env()
        self.providers = {}
        self._initialize_providers()
    
    def _initialize_providers(self):
        """Initialize available AI providers based on configuration"""
        available_keys = self.config.validate_keys()
        
        if available_keys["claude"] and self.config.anthropic_api_key:
            self.providers["claude"] = ClaudeProvider(
                api_key=self.config.anthropic_api_key,
                model=self.config.claude_model
            )
        
        if available_keys["gemini"] and self.config.google_api_key:
            self.providers["gemini"] = GeminiProvider(
                api_key=self.config.google_api_key,
                model=self.config.gemini_model
            )
        
        if available_keys["openai"] and self.config.openai_api_key:
            self.providers["openai"] = OpenAIProvider(
                api_key=self.config.openai_api_key,
                model=self.config.openai_model
            )
    
    async def ask(self, prompt: str, providers: Optional[List[str]] = None) -> Dict[str, str]:
        """Ask a question to multiple AI providers"""
        if providers is None:
            providers = list(self.providers.keys())
        
        tasks = []
        provider_names = []
        
        for provider_name in providers:
            if provider_name in self.providers:
                provider = self.providers[provider_name]
                task = provider.generate_response(
                    prompt,
                    max_tokens=self.config.max_tokens,
                    temperature=self.config.temperature
                )
                tasks.append(task)
                provider_names.append(provider_name)
        
        if not tasks:
            return {"error": "No valid providers available"}
        
        results = await asyncio.gather(*tasks, return_exceptions=True)
        
        responses = {}
        for i, result in enumerate(results):
            provider_name = provider_names[i]
            if isinstance(result, Exception):
                responses[provider_name] = f"Error: {str(result)}"
            else:
                responses[provider_name] = result
        
        return responses
    
    async def ask_claude(self, prompt: str) -> str:
        """Ask Claude specifically"""
        if "claude" not in self.providers:
            return "Claude provider not available"
        
        return await self.providers["claude"].generate_response(
            prompt,
            max_tokens=self.config.max_tokens,
            temperature=self.config.temperature
        )
    
    async def ask_gemini(self, prompt: str) -> str:
        """Ask Gemini specifically"""
        if "gemini" not in self.providers:
            return "Gemini provider not available"
        
        return await self.providers["gemini"].generate_response(
            prompt,
            max_tokens=self.config.max_tokens,
            temperature=self.config.temperature
        )
    
    async def ask_openai(self, prompt: str) -> str:
        """Ask OpenAI specifically"""
        if "openai" not in self.providers:
            return "OpenAI provider not available"
        
        return await self.providers["openai"].generate_response(
            prompt,
            max_tokens=self.config.max_tokens,
            temperature=self.config.temperature
        )
    
    def get_available_providers(self) -> List[str]:
        """Get list of available providers"""
        return list(self.providers.keys())
    
    def get_provider_info(self) -> Dict[str, Dict[str, Any]]:
        """Get information about all providers"""
        info = {}
        for name, provider in self.providers.items():
            info[name] = provider.get_provider_info()
        return info
